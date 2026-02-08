/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:50:53 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 14:35:55 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <string>
#include <vector>
#include <map>

#include "LocationConfig.hpp"

struct ServerConfig {
	std::vector<std::string> listen;
	std::map<int, std::string> error_pages;
	size_t client_max_body_size;
	std::vector<LocationConfig> locations;
};

#endif