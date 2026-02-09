/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:07:37 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 13:32:43 by aloiki           ###   ########.fr       */
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


Router::Router()
    : _root("./www"), _index("index.html"), _autoindex(true)
{}

HttpResponse Router::route(const HttpRequest &req)
{
    HttpResponse res;

    // Build filesystem path
    std::string fsPath = _root + req.path;

    // Check if path exists
    struct stat st;
    if (stat(fsPath.c_str(), &st) < 0)
    {
        res.status_code = 404;
        res.body = "404 Not Found";
        res.headers["Content-Length"] = "13";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // If directory
    if (S_ISDIR(st.st_mode))
        return serveDirectory(fsPath, req.path);

    // If file
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

HttpResponse Router::serveDirectory(const std::string &path, const std::string &urlPath)
{
    // Try index file
    std::string indexPath = path + "/" + _index;

    if (FileUtils::exists(indexPath))
    {
        return serveFile(indexPath);
    }

    // Autoindex?
    if (_autoindex)
    {
        return generateAutoindex(path, urlPath);
    }

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

        // html << "<li><a href=\"" << urlPath;
        // if (urlPath[urlPath.size() - 1] != '/')
        //     html << "/";
        // html << name << "\">" << name << "</a></li>";
    }

    html << "</ul>\n</body>\n</html>\n";
    closedir(dir);

    res.status_code = 200;
    res.body = html.str();
    res.headers["Content-Length"] = StringUtils::toString(res.body.size());
    res.headers["Content-Type"] = "text/html";

    return res;
}



