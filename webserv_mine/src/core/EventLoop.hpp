/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:49:07 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 14:51:36 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <vector>
#include <map>

class Client;

class EventLoop {
	public:
		EventLoop(const std::vector<int> &listeningSockets);
		~EventLoop();

		void run();
	private:
		std::vector<int> _listeningSockets;
		std::map<int, Client*> _clients;

		void acceptNewClient(int listenFd);
		void handleClientRead(int clientFd);
		void handleClientWrite(int clientFd);
		void removeClient(int clientFd);

};

#endif