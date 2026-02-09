/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:47:44 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 15:20:37 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <map>
#include "../config/ConfigParser.hpp"

class Server {
	public:
		Server(const std::vector<ServerConfig> &configs);
		~Server();

		void start();
	private:
		std::vector<int> _listeningSockets;
		std::vector<ServerConfig> _configs;
		int createListeningSocket(int port);

};

#endif