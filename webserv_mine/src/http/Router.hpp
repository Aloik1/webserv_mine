/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:54:08 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/10 15:01:25 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../config/ServerConfig.hpp"
#include "../config/LocationConfig.hpp"
#include <string>

class Router {
	public:
		Router(const ServerConfig &config);
		HttpResponse route(const HttpRequest &req);
	private:
		const ServerConfig &_config;
		const LocationConfig *matchLocation(const std::string &path);


		HttpResponse serveFile(const std::string &path);
		HttpResponse serveDirectory(const std::string &path, const std::string &urlPath, const std::string &index, bool autoindex);
		HttpResponse generateAutoindex(const std::string &path, const std::string &urlPath);
		HttpResponse handleDelete(const std::string &fsPath);
};

#endif