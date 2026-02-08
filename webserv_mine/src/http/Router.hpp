/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:54:08 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 15:38:46 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../config/ServerConfig.hpp"
#include <string>

class Router {
	public:
		Router();
		HttpResponse route(const HttpRequest &req);
	private:
		std::string _root;
		std::string _index;
		bool _autoindex;

		HttpResponse serveFile(const std::string &path);
		HttpResponse serveDirectory(const std::string &path, const std::string &urlPath);
		HttpResponse generateAutoindex(const std::string &path, const std::string &urlPath);
};

#endif