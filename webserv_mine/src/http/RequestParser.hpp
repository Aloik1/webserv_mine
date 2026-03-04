/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:53:34 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/12 16:04:04 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "HttpRequest.hpp"
#include "../config/ServerConfig.hpp"
#include <string>

#include "../core/Client.hpp"

class RequestParser {
	public:
		RequestParser();
		HttpRequest parse(const std::string &raw, const std::vector<ServerConfig> &eligibleConfigs, Client &c);
};

#endif