/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:07:22 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/02 15:57:48 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include "../utils/StringUtils.hpp"
#include "../http/HttpException.hpp"
#include "../config/ServerConfig.hpp"
#include <sstream>
#include <iostream>
#include <cstdlib>


#include <algorithm>

RequestParser::RequestParser() {}

HttpRequest RequestParser::parse(const std::string &raw, const std::vector<ServerConfig> &eligibleConfigs, Client &c)
{
    HttpRequest req;

    // 1. Find header delimiter
    size_t headerEnd = std::string::npos;
    size_t delimiterLen = 0;

    if (c.headerParsed) {
        headerEnd = c.headerSize;
        delimiterLen = c.headerDelimiterLen;
    } else {
        headerEnd = raw.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            headerEnd = raw.find("\n\n");
            delimiterLen = 2;
        } else {
            delimiterLen = 4;
        }

        if (headerEnd != std::string::npos) {
            c.headerParsed = true;
            c.headerSize = headerEnd;
            c.headerDelimiterLen = delimiterLen;
        }
    }

    if (headerEnd == std::string::npos)
    {
        size_t firstLineEnd = raw.find("\n");
        if (firstLineEnd != std::string::npos && firstLineEnd > 2048)
             throw HttpException(414, "URI Too Long");
        if (raw.size() > 8192)
        {
             // If we haven't found the header end yet, and the first line isn't finished, it's likely a huge URI
             if (firstLineEnd == std::string::npos)
                 throw HttpException(414, "URI Too Long");
             throw HttpException(431, "Request Header Fields Too Large");
        }
        return HttpRequest(); // incomplete
    }

    std::string headerPart = raw.substr(0, headerEnd);
    size_t bodyStart = headerEnd + delimiterLen;

    // Enforce first line limit (URI Too Long)
    size_t firstLineEnd = headerPart.find("\n");
    if (firstLineEnd != std::string::npos && firstLineEnd > 2048)
        throw HttpException(414, "URI Too Long");

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

    if (req.version != "HTTP/1.1")
        throw HttpException(505, "HTTP Version Not Supported");

    // EXTRA: split path and query string
    size_t qpos = req.path.find('?');
    if (qpos != std::string::npos)
    {
        req.query = req.path.substr(qpos + 1);
        req.path  = req.path.substr(0, qpos);
    }
    else
    {
        req.query = "";
    }


    // 3. Headers
    while (std::getline(ss, line))
    {
        line = StringUtils::trim(line);
        if (line.empty())
            continue;

        size_t colon = line.find(':');
        if (colon == std::string::npos)
            throw HttpException(400, "Bad Header");

        if (colon > 0 && (line[colon - 1] == ' ' || line[colon - 1] == '\t'))
             throw HttpException(400, "Bad Header (space before colon)");

        std::string key   = StringUtils::trim(line.substr(0, colon));
        if (key.empty())
             throw HttpException(400, "Bad Header (empty name)");

        std::string value = StringUtils::trim(line.substr(colon + 1));

        // Normalize header keys to lowercase
        std::string lowerKey = key;
        std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);

        if (req.headers.count(lowerKey))
             throw HttpException(400, "Duplicate Header");
        req.headers[lowerKey] = value;
    }

    if (req.headers.find("host") == req.headers.end())
         throw HttpException(400, "Host Header Required");

    if (req.headers["host"].find('@') != std::string::npos)
         throw HttpException(400, "Bad Request (Host header contains @)");

    // Select ServerConfig for this request to check client_max_body_size
    std::string host = req.headers["host"];
    size_t hostColon = host.find(':');
    if (hostColon != std::string::npos)
        host = host.substr(0, hostColon);

    const ServerConfig *config = &eligibleConfigs[0];
    for (size_t i = 0; i < eligibleConfigs.size(); ++i) {
        if (eligibleConfigs[i].server_name == host) {
            config = &eligibleConfigs[i];
            break;
        }
    }

    // Find matching location for client_max_body_size
    size_t maxBodySize = config->client_max_body_size;
    const LocationConfig *bestLoc = NULL;
    size_t bestLen = 0;
    for (size_t i = 0; i < config->locations.size(); ++i) {
        const LocationConfig &loc = config->locations[i];
        if (req.path.compare(0, loc.path.size(), loc.path) == 0) {
            if (loc.path.size() > bestLen) {
                bestLoc = &loc;
                bestLen = loc.path.size();
            }
        }
    }
    if (bestLoc && bestLoc->client_max_body_size > 0)
        maxBodySize = bestLoc->client_max_body_size;

    // 4. Body
    if (req.headers.count("transfer-encoding") && req.headers["transfer-encoding"] == "chunked")
    {
        std::string body;
        size_t pos = bodyStart;
        while (pos < raw.size())
        {
            size_t endOfSize = raw.find("\r\n", pos);
            if (endOfSize == std::string::npos)
                return HttpRequest(); // Incomplete: waiting for chunk size line

            std::string sizeStr = raw.substr(pos, endOfSize - pos);
            char *ptr;
            long chunkSize = strtol(sizeStr.c_str(), &ptr, 16);
            if (ptr != sizeStr.c_str() + sizeStr.size())
            {
                 // Check if it's a chunk extension (ignored for simplicity)
                 if (*ptr != ';')
                      throw HttpException(400, "Bad Request (invalid chunk size)");
            }

            if (chunkSize == 0)
            {
                // Last chunk: must be followed by optional trailers and final CRLF CRLF
                size_t trailerStart = endOfSize + 2;
                size_t finalDelimiter = raw.find("\r\n\r\n", endOfSize);
                if (finalDelimiter == std::string::npos)
                {
                    finalDelimiter = raw.find("\n\n", endOfSize);
                    if (finalDelimiter == std::string::npos)
                        return HttpRequest(); // Incomplete
                }

                if (finalDelimiter > trailerStart)
                {
                    std::string trailers = raw.substr(trailerStart, finalDelimiter - trailerStart);
                    std::istringstream ts(trailers);
                    std::string tline;
                    while (std::getline(ts, tline))
                    {
                        tline = StringUtils::trim(tline);
                        if (tline.empty()) continue;
                        size_t tcolon = tline.find(':');
                        if (tcolon == std::string::npos)
                             throw HttpException(400, "Bad Trailer");
                        
                        std::string tkey = StringUtils::toLower(StringUtils::trim(tline.substr(0, tcolon)));
                        std::string tval = StringUtils::trim(tline.substr(tcolon + 1));
                        req.headers[tkey] = tval;
                    }
                }

                req.body = body;
                req.consumedBytes = finalDelimiter + (raw.find("\r\n\r\n", endOfSize) != std::string::npos ? 4 : 2);
                return req;
            }

            // Check if we have the full chunk data + trailing CRLF
            if (raw.size() < endOfSize + 2 + chunkSize + 2)
                return HttpRequest();

            body.append(raw.substr(endOfSize + 2, chunkSize));

            if (body.size() > maxBodySize)
                throw HttpException(413, "Payload Too Large");

            pos = endOfSize + 2 + chunkSize + 2;
        }
        return HttpRequest(); // Incomplete: waiting for more chunks
    }
    else if (req.headers.count("content-length"))
    {
        std::string lengthStr = req.headers["content-length"];
        if (lengthStr.empty() || lengthStr.find_first_not_of("0123456789") != std::string::npos)
             throw HttpException(400, "Bad Request (invalid content-length)");

        if (lengthStr.size() > 15) // Max size_t is usually 20 digits, but we can be safer
             throw HttpException(400, "Bad Request (content-length too large)");

        long len = std::atol(lengthStr.c_str());

        if (len > (long)maxBodySize)
        {
            throw HttpException(413, "Payload Too Large");
        }

        if (raw.size() < bodyStart + (size_t)len)
            return HttpRequest(); // incomplete

        req.body = raw.substr(bodyStart, len);
        req.consumedBytes = bodyStart + len;
    }
    else
    {
        req.consumedBytes = bodyStart;
    }
    return req;
}