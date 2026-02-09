/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:48:24 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 15:27:08 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "../config/ServerConfig.hpp"

class Client {
	public:
		int fd;
		std::string readBuffer;
		std::string writeBuffer;
		bool wantWrite;

		ServerConfig config;

		Client(int fd, const ServerConfig &conf);
		~Client();

};

#endif