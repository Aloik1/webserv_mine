/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:03:59 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 16:37:43 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include "../http/RequestParser.hpp"
#include "../http/Router.hpp"
#include "Client.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <algorithm>


EventLoop::EventLoop(const std::vector<int> &listeningSockets)
    : _listeningSockets(listeningSockets)
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
        for (size_t i = 0; i < _listeningSockets.size(); i++) {
            pollfd p;
            p.fd = _listeningSockets[i];
            p.events = POLLIN;
            p.revents = 0;
            fds.push_back(p);
        }

        // Add client sockets
        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
            pollfd p;
            p.fd = it->first;
            p.events = POLLIN;
            if (!_clients[it->first]->writeBuffer.empty())
                p.events |= POLLOUT;
            p.revents = 0;
            fds.push_back(p);
        }

        int ret = poll(&fds[0], fds.size(), 1000);
        if (ret < 0) {
            std::cerr << "poll() failed\n";
            continue;
        }
        // Process events
        for (size_t i = 0; i < fds.size(); i++) {
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
                handleClientRead(fd);

            if (_clients.find(fd) != _clients.end() && (fds[i].revents & POLLOUT))
                handleClientWrite(fd);

            if (fds[i].revents & (POLLHUP | POLLERR))
                removeClient(fd);
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

    _clients[clientFd] = new Client(clientFd);

    std::cout << "New client connected: " << clientFd << "\n";
}

void EventLoop::handleClientRead(int clientFd) 
{
    char buffer[4096];
    int bytes = recv(clientFd, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
    {
        removeClient(clientFd);
        return;
    }
    Client *c = _clients[clientFd];
    c->readBuffer.append(buffer, bytes);

    // Try to parse request
    RequestParser parser;
    HttpRequest req = parser.parse(c->readBuffer);
    
    // Check if request is complete
    /*if (!req.method.empty())
    {
        Router router;
        HttpResponse res = router.route(req);

        c->writeBuffer = res.serialize();
        c->readBuffer.clear();
    }*/
    if (!req.method.empty())
    {
        Router router;
        HttpResponse res = router.route(req);

        std::string raw = res.serialize();

        std::cout << "=== ROUTER RESPONSE BEGIN ===\n";
        std::cout << raw << "\n";
        std::cout << "=== ROUTER RESPONSE END ===\n";

        c->writeBuffer = raw;
        c->readBuffer.clear();
    }


}

void EventLoop::handleClientWrite(int clientFd)
{
    Client *c = _clients[clientFd];

    if (c->writeBuffer.empty())
        return;

    int bytes = send(clientFd, c->writeBuffer.c_str(), c->writeBuffer.size(), 0);

    if (bytes <= 0)
    {
        removeClient(clientFd);
        return;
    }

    c->writeBuffer.erase(0, bytes);
    // If everything was sent, close the connection
    if (c->writeBuffer.empty())
    {
        removeClient(clientFd);
    }

}

void EventLoop::removeClient(int clientFd)
{
    std::cout << "Client disconnected: " << clientFd << "\n";

    delete _clients[clientFd];
    _clients.erase(clientFd);
}

