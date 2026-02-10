/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:51:37 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/10 14:43:37 by aloiki           ###   ########.fr       */
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
	
	// List of allowed HTTP methods for this location (e.g. GET, POST, DELETE).
    	// If empty, all methods are allowed.
	std::vector<std::string> methods;

	// Directory where uploaded files should be stored (for POST).
	std::string upload_store;

	// CGI configuration: extension and interpreter path.
	std::string cgi_extension;
	std::string cgi_path;
	
	// Optional redirect: if redirect_code != 0, redirect to redirect_url.
	int redirect_code;
	std::string redirect_url;

	LocationConfig() : autoindex(false), redirect_code(0) {}
};

#endif