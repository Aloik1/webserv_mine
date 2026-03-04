/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:03:44 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/02 18:30:16 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <unistd.h>

Client::Client(int fd, const std::vector<ServerConfig> &eligibleConfigs)
    : fd(fd),
      wantWrite(false),
      configs(eligibleConfigs),
      defaultConfig(eligibleConfigs[0]),
      keepAlive(false),
      lastActivity(time(NULL)),
      headerParsed(false),
      headerSize(0),
      headerDelimiterLen(0)
{}

Client::~Client()
{
	close(fd);
}