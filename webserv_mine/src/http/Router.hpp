/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:54:08 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/02 17:45:47 by aloiki           ###   ########.fr       */
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
		Router(const std::vector<ServerConfig> &servers);
		HttpResponse route(const HttpRequest &req);
	private:
		std::vector<ServerConfig> _servers;
		const LocationConfig *matchLocation(const std::string &path, const ServerConfig &config);
		const ServerConfig& selectServer(const HttpRequest &req) const;


		HttpResponse serveFile(const std::string &path, const ServerConfig &config);
		HttpResponse serveDirectory(const std::string &path, const std::string &urlPath, const std::string &index, bool autoindex, const ServerConfig &config);
		HttpResponse generateAutoindex(const std::string &path, const std::string &urlPath);
		HttpResponse handleDelete(const std::string &fsPath, const LocationConfig *loc, const ServerConfig &config);
		HttpResponse handlePost(const HttpRequest &req, const LocationConfig *loc, const ServerConfig &config);
		HttpResponse handlePut(const HttpRequest &req, const std::string &fsPath, const LocationConfig *loc, const ServerConfig &config);
		HttpResponse parseCgiResponse(const std::string &raw, const ServerConfig &config);
		HttpResponse generateAutoindexResponse(const std::string &urlPath, const std::string &fsPath, const ServerConfig &config);
		HttpResponse tryContentNegotiation(const std::string &fsPath, const HttpRequest &req, const ServerConfig &config);
		HttpResponse makeErrorResponse(int code, const std::string &msg, const ServerConfig &config);
		bool checkAuth(const HttpRequest &req, const LocationConfig *loc, HttpResponse &res);

};

#endif