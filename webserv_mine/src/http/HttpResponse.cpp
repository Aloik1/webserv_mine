/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:06:54 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 15:45:36 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include <sstream>

HttpResponse::HttpResponse()
	: status_code(200)
{}

std::string HttpResponse::serialize() const
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << status_code << " ";

    switch (status_code) {
        case 200: ss << "OK"; break;
        case 403: ss << "Forbidden"; break;
        case 404: ss << "Not Found"; break;
        case 500: ss << "Internal Server Error"; break;
        default: ss << "OK"; break;
    }

    ss << "\r\n";

    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it)
    {
        ss << it->first << ": " << it->second << "\r\n";
    }

    ss << "\r\n";
    ss << body;

    return ss.str();
}
