/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:55:00 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/28 15:50:09 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <string>
#include "../http/HttpRequest.hpp"

class CgiHandler {
	public:
		CgiHandler();
		std::string execute(const std::string &script, const HttpRequest &req, const std::string &interpreter);
};

#endif