/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:53:34 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 13:53:46 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "HttpRequest.hpp"
#include <string>

class RequestParser {
	public:
		RequestParser();
		HttpRequest parse(const std::string &raw);
};

#endif