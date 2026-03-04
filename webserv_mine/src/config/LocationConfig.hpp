/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:51:37 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/01 18:14:50 by aloiki           ###   ########.fr       */
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
	bool is_alias;

	
	// List of allowed HTTP methods for this location (e.g. GET, POST, DELETE).
    	// If empty, all methods are allowed.
	std::vector<std::string> methods;

	// Directory where uploaded files should be stored (for POST).
	std::string upload_store;

	// CGI configuration: extension and interpreter path.
	std::string cgi_extension;
	std::string cgi_path;

	std::string auth_basic;
	std::string auth_basic_user_file;
	
	// Optional redirect: if redirect_code != 0, redirect to redirect_url.
	int redirect_code;
	std::string redirect_url;

	size_t client_max_body_size;

	LocationConfig() : path(""), root(""), index(""), autoindex(false), is_alias(false), upload_store(""), cgi_extension(""), cgi_path(""), auth_basic(""), redirect_code(0), redirect_url(""), client_max_body_size(0) {}
};

#endif