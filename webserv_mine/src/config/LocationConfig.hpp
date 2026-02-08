/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:51:37 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 13:51:48 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <string>
#include <vector>

struct LocationConfig {
	std::string path;
	std::string root;
	std::string index;
	bool autoindex;
	std::vector<std::string> methods;
	std::string upload_store;
	std::string cgi_extension;
	std::string cgi_path;
	int redirect_code;
	std::string redirect_url;

	LocationConfig() : autoindex(false), redirect_code(0) {}
};

#endif