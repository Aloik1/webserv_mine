/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:03:44 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 15:27:45 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <unistd.h>

Client::Client(int fd, const ServerConfig &conf)
	: fd(fd), wantWrite(false), config(conf)
{}

Client::~Client()
{
	close(fd);
}