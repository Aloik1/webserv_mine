/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:07:37 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/10 14:47:47 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "../utils/FileUtils.hpp"
#include "../utils/StringUtils.hpp"
#include "../utils/MimeTypes.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <sstream>
#include <iostream>
#include <cerrno>


Router::Router(const ServerConfig &config)
    : _config(config)
{}

HttpResponse Router::route(const HttpRequest &req)
{
    HttpResponse res;

    // 1. Find matching location
    const LocationConfig *loc = matchLocation(req.path);

    // 1.1 Enforce allowed methods
    if (loc && !loc->methods.empty())
    {
        bool allowed = false;
        for (size_t i = 0; i < loc->methods.size(); ++i)
        {
            if (loc->methods[i] == req.method)
            {
                allowed = true;
                break;
            }
        }
        if (!allowed)
        {
            res.status_code = 405;
            res.body = "405 Method Not Allowed";
            res.headers["Content-Length"] = StringUtils::toString(res.body.size());
            res.headers["Content-Type"] = "text/plain";

            // Build Allow header
            std::string allow;
            for (size_t i = 0; i < loc->methods.size(); ++i)
            {
                if (i > 0) allow += ", ";
                allow += loc->methods[i];
            }
            res.headers["Allow"] = allow;

            return res;
        }
    }
    // 2. Determine effective root
    std::string root = loc ? loc->root : _config.root;

    // 3. Compute the effective path (strip location prefix)
    std::string path = req.path;

    if (loc)
    {
        // Remove the location prefix
        if (path.size() >= loc->path.size())
            path = path.substr(loc->path.size());

        // If nothing left, treat as "/"
        if (path.empty())
            path = "/";
    }
    // 4. Build filesystem path
    std::string fsPath = root + path;

    // 5. Stat the path
    std::cout << "[DEBUG] fsPath = '" << fsPath << "'" << std::endl;
    struct stat st;
    if (stat(fsPath.c_str(), &st) < 0)
    {
        // Custom error page?
        if (_config.error_pages.count(404))
        {
            std::string errPath = _config.root + _config.error_pages.at(404);
            return serveFile(errPath);
        }

        res.status_code = 404;
        res.body = "404 Not Found";
        res.headers["Content-Length"] = "13";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }
    std::cout << "[DEBUG] st_mode = " << st.st_mode
          << " isDir = " << S_ISDIR(st.st_mode) << std::endl;

     // 6. Directory?
    if (S_ISDIR(st.st_mode))
    {
        std::cout << "Entered directory if" << std::endl;
        bool autoindex = loc ? loc->autoindex : _config.autoindex;
        std::string index = (loc && !loc->index.empty()) ? loc->index : _config.index;

        return serveDirectory(fsPath, path, index, autoindex);
    }

    // 7. File
    return serveFile(fsPath);
}


HttpResponse Router::serveFile(const std::string &path)
{
    HttpResponse res;

    if (!FileUtils::exists(path))
    {
        res.status_code = 404;
        res.body = "404 Not Found";
        res.headers["Content-Length"] = "13";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    std::string content = FileUtils::readFile(path);

    // Detect MIME type
    size_t dot = path.find_last_of('.');
    std::string ext = (dot != std::string::npos) ? path.substr(dot) : "";
    std::string mime = MimeTypes::get(ext);

    res.status_code = 200;
    res.body = content;
    res.headers["Content-Length"] = StringUtils::toString(content.size());
    res.headers["Content-Type"] = mime;

    return res;
}

HttpResponse Router::serveDirectory(const std::string &path, const std::string &urlPath, const std::string &index, bool autoindex)
{
    // Try index file
    std::string indexPath = path + "/" + index;

    //
    std::cout << "indexPath = '" << indexPath << "'" << std::endl;
    struct stat st2;
    int r = stat(indexPath.c_str(), &st2);
    std::cout << "stat(indexPath) = " << r << " errno=" << errno << std::endl;
    //

    if (FileUtils::exists(indexPath))
    {
        std::cout << "Its a FILE" << std::endl;
        return serveFile(indexPath);
    }

    // Autoindex?
    if (autoindex)
        return generateAutoindex(path, urlPath);

    // Forbidden
    HttpResponse res;
    res.status_code = 403;
    res.body = "403 Forbidden";
    res.headers["Content-Length"] = "13";
    res.headers["Content-Type"] = "text/plain";
    return res;
}

HttpResponse Router::generateAutoindex(const std::string &path, const std::string &urlPath)
{
    HttpResponse res;

    DIR *dir = opendir(path.c_str());
    if (!dir)
    {
        res.status_code = 500;
        res.body = "500 Internal Server Error";
        res.headers["Content-Length"] = "25";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    std::stringstream html;
    html << "<!DOCTYPE html>\n"
     << "<html>\n<head>\n"
     << "<meta charset=\"utf-8\">\n"
     << "<title>Index of " << urlPath << "</title>\n"
     << "</head>\n<body>\n";

    html << "<h1>Index of " << urlPath << "</h1>\n";
    html << "<ul>\n";

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;
        std::string href = urlPath;
        if (href.empty() || href[href.size() - 1] != '/')
            href += "/";
        href += name;
        html << "<li><a href=\"" << href << "\">" << name << "</a></li>";
    }

    html << "</ul>\n</body>\n</html>\n";
    closedir(dir);

    res.status_code = 200;
    res.body = html.str();
    res.headers["Content-Length"] = StringUtils::toString(res.body.size());
    res.headers["Content-Type"] = "text/html";

    return res;
}

const LocationConfig *Router::matchLocation(const std::string &path)
{
    const LocationConfig *best = NULL;
    size_t bestLen = 0;

    for (size_t i = 0; i < _config.locations.size(); i++)
    {
        const LocationConfig &loc = _config.locations[i];

        // Must be a prefix match
        if (path.compare(0, loc.path.size(), loc.path) == 0)
        {
            // Longest prefix wins
            if (loc.path.size() > bestLen)
            {
                best = &loc;
                bestLen = loc.path.size();
            }
        }
    }
    std::cout << "[DEBUG] matchLocation('" << path << "') → "
        << (best ? best->path : "NONE") << std::endl;

    return best;
}


