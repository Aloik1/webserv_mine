/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:07:22 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/12 17:16:22 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include "../utils/StringUtils.hpp"
#include "../http/HttpException.hpp"
#include "../config/ServerConfig.hpp"
#include <sstream>
#include <iostream>
#include <cstdlib>


RequestParser::RequestParser() {}

HttpRequest RequestParser::parse(const std::string &raw, const ServerConfig &config)
{
    HttpRequest req;

    // 1. Find header delimiter
    size_t headerEnd = raw.find("\r\n\r\n");
    size_t delimiterLen = 4;

    if (headerEnd == std::string::npos) {
        headerEnd = raw.find("\n\n");
        delimiterLen = 2;
    }

    if (headerEnd == std::string::npos)
        return HttpRequest(); // incomplete

    std::string headerPart = raw.substr(0, headerEnd);
    std::string bodyPart   = raw.substr(headerEnd + delimiterLen);

    std::stringstream ss(headerPart);
    std::string line;

    // 2. Request line
    if (!std::getline(ss, line))
        return HttpRequest(); // incomplete

    line = StringUtils::trim(line);
    std::vector<std::string> parts = StringUtils::split(line, ' ');

    if (parts.size() != 3)
        throw HttpException(400, "Bad Request");

    req.method  = parts[0];
    req.path    = parts[1];
    req.version = parts[2];

    // 3. Headers
    while (std::getline(ss, line))
    {
        line = StringUtils::trim(line);
        if (line.empty())
            continue;

        size_t colon = line.find(':');
        if (colon == std::string::npos)
            throw HttpException(400, "Bad Header");

        std::string key   = StringUtils::trim(line.substr(0, colon));
        std::string value = StringUtils::trim(line.substr(colon + 1));

        req.headers[key] = value;
    }

    // 4. Body (Content-Length)
    if (req.headers.find("Content-Length") != req.headers.end())
    {
        int len = std::atoi(req.headers["Content-Length"].c_str());

        if (bodyPart.size() < (size_t)len)
            return HttpRequest(); // incomplete

        if (len > (int)config.client_max_body_size)
            throw HttpException(413, "Payload Too Large");

        req.body = bodyPart.substr(0, len);
    }

    return req;
}

/*HttpRequest RequestParser::parse(const std::string &raw, const ServerConfig &config)
{
    HttpRequest req;

    // 1. Split headers and body
    size_t headerEnd = raw.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        headerEnd = raw.find("\n\n");
    if (headerEnd == std::string::npos)
        return HttpRequest(); // still incomplete


    std::string headerPart = raw.substr(0, headerEnd);
    size_t delimiterLen = 4;
    if (raw.find("\r\n\r\n") == std::string::npos)
        delimiterLen = 2;

    std::string bodyPart = raw.substr(headerEnd + delimiterLen);

    // std::string bodyPart = raw.substr(headerEnd + 4);

    std::stringstream ss(headerPart);
    std::string line;

    // 2. Parse request line
    if (!std::getline(ss, line))
    {
        return HttpRequest();
    }

    line = StringUtils::trim(line);
    std::vector<std::string> parts = StringUtils::split(line, ' ');

    if (parts.size() != 3)
        throw HttpException(400, "Bad Request");

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

        // If body not fully received → request is incomplete
        if (bodyPart.size() < (size_t)len)
            return HttpRequest(); // empty method → EventLoop waits for more data

        // Now enforce client_max_body_size
        if (len > (int)config.client_max_body_size)
            throw HttpException(413, "Payload Too Large");

        req.body = bodyPart.substr(0, len);
    }

   
    return req;
}*/
    // if (req.headers.find("Content-Length") != req.headers.end()) {
    //     int len = std::atoi(req.headers["Content-Length"].c_str());
        
    //     // FIRST LIMIT CHECK — based on Content-Length header
    //     if (len > (int)config.client_max_body_size)
    //         throw HttpException(413, "Payload Too Large");

    //     // If full body received
    //     if (bodyPart.size() >= (size_t)len)
    //     {
    //         // SECOND LIMIT CHECK — in case client lies or sends more
    //         if (len > (int)config.client_max_body_size)
    //             throw HttpException(413, "Payload Too Large");

    //         req.body = bodyPart.substr(0, len);
    //     }

    // }