/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:49:07 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 15:44:44 by aloiki           ###   ########.fr       */
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
		EventLoop(const std::vector<int> &listeningSockets, const std::vector<ServerConfig> &configs);
		~EventLoop();

		void run();
	private:
		std::vector<int> _listeningSockets;
		std::vector<ServerConfig> _configs;
		std::map<int, Client*> _clients;

		void acceptNewClient(int listenFd);
		void handleClientRead(int clientFd);
		void handleClientWrite(int clientFd);
		void removeClient(int clientFd);

};

#endif