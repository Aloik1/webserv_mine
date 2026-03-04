/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:48:24 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/02 18:31:43 by aloiki           ###   ########.fr       */
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

		std::vector<ServerConfig> configs;
		ServerConfig defaultConfig;

		bool keepAlive;
		time_t lastActivity;

		bool headerParsed;
		size_t headerSize;
		size_t headerDelimiterLen;

		Client(int fd, const std::vector<ServerConfig> &eligibleConfigs);
		~Client();
};

#endif