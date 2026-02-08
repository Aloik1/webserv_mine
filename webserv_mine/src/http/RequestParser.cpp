/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:07:22 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 16:03:01 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include "../utils/StringUtils.hpp"
#include <sstream>
#include <iostream>
#include <cstdlib>


RequestParser::RequestParser() {}

HttpRequest RequestParser::parse(const std::string &raw)
{
    HttpRequest req;

    // 1. Split headers and body
    size_t headerEnd = raw.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        headerEnd = raw.find("\n\n");
    if (headerEnd == std::string::npos)
        return req; // still incomplete


    std::string headerPart = raw.substr(0, headerEnd);
    std::string bodyPart = raw.substr(headerEnd + 4);

    std::stringstream ss(headerPart);
    std::string line;

    // 2. Parse request line
    if (!std::getline(ss, line))
    {
        return req;
    }

    line = StringUtils::trim(line);
    std::vector<std::string> parts = StringUtils::split(line, ' ');

    if (parts.size() != 3)
        return req;

    req.method = parts[0];
    req.path = parts[1];
    req.version = parts[2];

    // 3. Parse headers
    while (std::getline(ss, line))
    {
        line = StringUtils::trim(line);
        if (line.empty())
            continue;

        size_t colon = line.find(':');
        if (colon == std::string::npos)
            continue;

        std::string key = StringUtils::trim(line.substr(0, colon));
        std::string value = StringUtils::trim(line.substr(colon + 1));

        req.headers[key] = value;
    }
    // 4. Parse body (Content-Length only for now)
    if (req.headers.find("Content-Length") != req.headers.end()) {
        int len = std::atoi(req.headers["Content-Length"].c_str());
        if (bodyPart.size() >= (size_t)len) {
            req.body = bodyPart.substr(0, len);
        }
    }
    return req;
}