/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:47:44 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 14:39:49 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <map>

class Server {
	public:
		Server();
		~Server();

		void start();
	private:
		std::vector<int> _listeningSockets;

		int createListeningSocket(int port);

};

#endif