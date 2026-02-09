/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:50:53 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 15:52:35 by aloiki           ###   ########.fr       */
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
    std::string server_name;
    std::string root;
    std::string index;
    bool autoindex;

    std::map<int, std::string> error_pages;
    size_t client_max_body_size;

    std::vector<LocationConfig> locations;

    ServerConfig()
        : server_name(""), root("./www"), index("index.html"), autoindex(false), client_max_body_size(0)
    {}

};

#endif