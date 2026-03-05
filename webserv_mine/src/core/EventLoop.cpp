/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:03:59 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/02 18:49:27 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include "../http/RequestParser.hpp"
#include "../http/Router.hpp"
#include "Client.hpp"
#include "../utils/StringUtils.hpp"
#include "../http/HttpException.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <algorithm>
#include <errno.h>
#include <string.h>

EventLoop::EventLoop(const std::vector<int> &listeningSockets, const std::map<int, std::vector<ServerConfig> > &socketToConfigs)
    : _listeningSockets(listeningSockets), _socketToConfigs(socketToConfigs)
{}


EventLoop::~EventLoop()
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete it->second;
}

void EventLoop::run() 
{
    std::vector<pollfd> fds;
    while (true)
    {
        fds.clear();

        // Add listening sockets
        for (size_t i = 0; i < _listeningSockets.size(); i++)
        {
            pollfd p;
            p.fd = _listeningSockets[i];
            p.events = POLLIN;
            p.revents = 0;
            fds.push_back(p);
        }

        // Add client sockets
        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            pollfd p;
            p.fd = it->first;
            p.events = POLLIN;
            if (_clients[it->first]->wantWrite)
                p.events |= POLLOUT;
            p.revents = 0;
            fds.push_back(p);
        }

        // --- KEEP ALIVE TIMEOUT CHECK ---
        time_t now = time(NULL);
        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end();)
        {
            Client* c = it->second;
            if (now - c->lastActivity > 60)
            {
                int fd = it->first;
                ++it; // Move iterator before removing the client to avoid invalidation
                removeClient(fd);
            }
            else
            {
                ++it;
            }
        }
        // --- END KEEP ALIVE TIMEOUT CHECK ---

        int ret = poll(&fds[0], fds.size(), 1000);
        if (ret < 0)
        {
            std::cerr << "poll() failed\n";
            continue;
        }

        // Process events
        for (size_t i = 0; i < fds.size(); i++)
        {
            int fd = fds[i].fd;

            // Listening socket → accept new client
            if (std::find(_listeningSockets.begin(), _listeningSockets.end(), fd) != _listeningSockets.end())
            {
                if (fds[i].revents & POLLIN)
                    acceptNewClient(fd);
                continue;
            }

            // Client socket
            if (fds[i].revents & POLLIN)
            {
                handleClientRead(fd);
            }


            if (_clients.find(fd) != _clients.end() && (fds[i].revents & POLLOUT))
            {
                handleClientWrite(fd);
            }

            // Only close on POLLERR.
            // POLLHUP means the client won't send more data,
            // but we can still write the response.
            if (fds[i].revents & POLLERR)
            {
                removeClient(fd);
            }

        }
    }
}

void EventLoop::acceptNewClient(int listenFd)
{
    int clientFd = accept(listenFd, NULL, NULL);
    if (clientFd < 0)
        return;

    // Make non-blocking
    fcntl(clientFd, F_SETFL, O_NONBLOCK);

    // Get the correct server configs for this listening socket
    const std::vector<ServerConfig> &eligibleConfigs = _socketToConfigs.at(listenFd);

    // Create client with the correct configs
    _clients[clientFd] = new Client(clientFd, eligibleConfigs);

    std::cout << "[INFO] New client connected on fd " << clientFd << std::endl;
}


void EventLoop::handleClientRead(int clientFd) 
{
    char buffer[8192];
    int bytes = recv(clientFd, buffer, sizeof(buffer), 0);

    if (bytes < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        removeClient(clientFd);
        return;
    }
    if (bytes == 0)
    {
        removeClient(clientFd);
        return;
    }

    Client *c = _clients[clientFd];
    c->readBuffer.append(buffer, bytes);
    c->lastActivity = time(NULL);

    // Limit buffer size to prevent memory exhaustion
    // 10MB (max body) + 16KB (headers/buffer room)
    if (c->readBuffer.size() > c->defaultConfig.client_max_body_size + 16384)
    {
        std::cerr << "[ERROR] Client buffer exceeded maximum allowed size (" << c->readBuffer.size() << " bytes). Disconnecting.\n";
        
        HttpResponse res;
        res.status_code = 413;
        res.body = "Payload Too Large";
        res.headers["Content-Length"] = StringUtils::toString(res.body.size());
        res.headers["Connection"] = "close";
        
        c->writeBuffer = res.serialize();
        c->wantWrite = true;
        c->readBuffer.clear();
        c->keepAlive = false;
        return;
    }

    processRequests(c);
}

void EventLoop::processRequests(Client *c)
{
    RequestParser parser;
    while (!c->readBuffer.empty())
    {
        HttpRequest req;
        try
        {
            req = parser.parse(c->readBuffer, c->configs, *c);
        }
        catch (const HttpException &e)
        {
            HttpResponse res;
            res.status_code = e.code;
            res.body = e.message;
            res.headers["Content-Length"] = StringUtils::toString(res.body.size());
            res.headers["Content-Type"] = "text/plain";
            res.headers["Connection"] = "close";

            c->writeBuffer += res.serialize();
            c->wantWrite = true;
            c->keepAlive = false;
            c->readBuffer.clear();
            return;
        }

        if (req.method.empty() || req.consumedBytes == 0)
            break;

        std::cout << "[INFO] Request: " << req.method << " " << req.path << " (fd " << c->fd << ")" << std::endl;

        // Keep-Alive Detection
        if (req.headers.count("connection"))
        {
            std::string conn = req.headers.at("connection");
            std::transform(conn.begin(), conn.end(), conn.begin(), ::tolower);
            c->keepAlive = (conn == "keep-alive");
        }
        else
        {
            c->keepAlive = true;
        }

        Router router(c->configs);
        HttpResponse res;
        try
        {
            res = router.route(req);
        }
        catch (const std::exception &e)
        {
            res.status_code = 500;
            res.body = "Internal Server Error";
            res.headers["Content-Length"] = StringUtils::toString(res.body.size());
            res.headers["Connection"] = "close";
        }
        
        std::cout << "[INFO] Response: " << res.status_code << " (fd " << c->fd << ")" << std::endl;
        
        c->writeBuffer += res.serialize();
        c->wantWrite = true;
        c->readBuffer.erase(0, req.consumedBytes);
        c->headerParsed = false;
        c->headerSize = 0;

        if (!c->keepAlive)
            break;
    }
}

void EventLoop::handleClientWrite(int clientFd)
{
    Client *c = _clients[clientFd];

    if (c->writeBuffer.empty())
    {
        c->wantWrite = false;
        return;
    }

    ssize_t bytes = send( clientFd, c->writeBuffer.c_str(), c->writeBuffer.size(),
    #ifdef MSG_NOSIGNAL
        MSG_NOSIGNAL          // avoid SIGPIPE on Linux
    #else
        0
    #endif
    );
    c->lastActivity = time(NULL);
    if (bytes <= 0)
    {
        removeClient(clientFd);
        return;
    }

    if (bytes == 0)
    {
        // peer closed, nothing more to do
        removeClient(clientFd);
        return;
    }
    c->writeBuffer.erase(0, bytes);
    if (c->writeBuffer.empty())
    {
        c->wantWrite = false;

        if (!c->keepAlive)
        {
            removeClient(clientFd);
        }
        else
        {
            c->lastActivity = time(NULL);
        }
    }

}

void EventLoop::removeClient(int clientFd)
{
    std::cout << "[INFO] Client disconnected on fd " << clientFd << std::endl;

    delete _clients[clientFd];
    _clients.erase(clientFd);
}

