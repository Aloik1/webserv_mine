/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:03:44 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 16:48:05 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <unistd.h>

Client::Client(int fd)
	: fd(fd), wantWrite(false)
{}

Client::~Client()
{
	close(fd);
}