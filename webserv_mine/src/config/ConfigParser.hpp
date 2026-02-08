/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:49:49 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 13:50:06 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <vector>

class ServerConfig; // forward declaration

class ConfigParser {
	public:
		ConfigParser();
		~ConfigParser();

		std::vector<ServerConfig> parse(const std::string &path);
};

#endif