/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:49:07 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/02 16:55:26 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <vector>
#include <map>
#include "../config/ServerConfig.hpp"

class Client;

class EventLoop {
	public:
		EventLoop(const std::vector<int> &listeningSockets, const std::map<int, std::vector<ServerConfig> > &socketToConfigs);
		~EventLoop();
		void run();
	private:
		std::vector<int> _listeningSockets;
		std::map<int, Client*> _clients;
		std::map<int, std::vector<ServerConfig> > _socketToConfigs;

		void acceptNewClient(int listenFd);
		void handleClientRead(int clientFd);
		void handleClientWrite(int clientFd);
		void processRequests(Client *c);
		void removeClient(int clientFd);

};

#endif