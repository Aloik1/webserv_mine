/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:06:54 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/01 17:29:44 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include <sstream>
#include <ctime>

HttpResponse::HttpResponse()
	: status_code(200), is_head(false)
{}

std::string HttpResponse::serialize() const
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << status_code << " ";

    switch (status_code)
    {
        case 200: ss << "OK"; break;
        case 201: ss << "Created"; break;
        case 202: ss << "Accepted"; break;
        case 204: ss << "No Content"; break;
        case 301: ss << "Moved Permanently"; break;
        case 302: ss << "Found"; break;
        case 400: ss << "Bad Request"; break;
        case 401: ss << "Unauthorized"; break;
        case 403: ss << "Forbidden"; break;
        case 404: ss << "Not Found"; break;
        case 405: ss << "Method Not Allowed"; break;
        case 413: ss << "Payload Too Large"; break;
        case 414: ss << "URI Too Long"; break;
        case 431: ss << "Request Header Fields Too Large"; break;
        case 500: ss << "Internal Server Error"; break;
        case 501: ss << "Not Implemented"; break;
        case 505: ss << "HTTP Version Not Supported"; break;
        default: ss << "Unknown Status"; break;
    }

    ss << "\r\n";

    if (headers.find("Server") == headers.end())
        ss << "Server: webserv/1.0\r\n";
    
    if (headers.find("Date") == headers.end())
    {
        char buf[100];
        time_t now = time(0);
        struct tm tm = *gmtime(&now);
        strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &tm);
        ss << "Date: " << buf << "\r\n";
    }

    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it)
    {
        ss << it->first << ": " << it->second << "\r\n";
    }

    ss << "\r\n";
    if (!is_head)
        ss << body;

    return ss.str();
}
