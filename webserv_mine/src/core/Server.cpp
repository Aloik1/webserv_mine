/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:03:23 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 14:59:57 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "EventLoop.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>


Server::Server()
{}

Server::~Server()
{
    for (size_t i = 0; i < _listeningSockets.size(); i++)
        close(_listeningSockets[i]);

}

int Server::createListeningSocket(int port) 
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        std::cerr << "socket() failed\n";
        return -1;
    }
    // Allow immediate reuse of the port
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    {
        std::cerr << "setsockopt() failed\n";
        close(sockfd);
        return -1;
    }
    // Make socket non-blocking
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl() failed\n";
        close(sockfd);
        return -1;
    }
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces
    addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "bind() failed on port " << port << "\n";
        close(sockfd);
        return -1;
    }
    if (listen(sockfd, 128) < 0)
    {
        std::cerr << "listen() failed\n";
        close(sockfd);
        return -1;
    }
    std::cout << "Listening on port " << port << "...\n";
    return sockfd;
}


void Server::start() {
    int port = 8080;

    int sock = createListeningSocket(port);
    if (sock < 0) {
        std::cerr << "Failed to create listening socket\n";
        return;
    }

    _listeningSockets.push_back(sock);

    std::cout << "Server started. Waiting for connections...\n";

    EventLoop loop(_listeningSockets);
    loop.run();
}

