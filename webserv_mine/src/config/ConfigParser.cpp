/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:05:01 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 14:32:22 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

ConfigParser::ConfigParser()
{}

ConfigParser::~ConfigParser()
{}

std::vector<ServerConfig> ConfigParser::parse(const std::string &path)
{
    std::string test = path;
    std::vector<ServerConfig> servers;

    // TODO: Implement config parsing logic

    return servers;
}