/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:07:37 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/01 15:31:49 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "../utils/FileUtils.hpp"
#include "../utils/StringUtils.hpp"
#include "../utils/MimeTypes.hpp"
#include "../cgi/CgiHandler.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <cstdio>
#include <cstdlib>

Router::Router(const ServerConfig &config)
    : _config(config)
{}

HttpResponse Router::route(const HttpRequest &req)
{
    HttpResponse res;
    std::cout << "[DEBUG] route(): req.path = '" << req.path << "'\n";
    std::cout << "[DEBUG] req.method = '" << req.method << "'" << std::endl;


    // 1. Find matching location    
    const LocationConfig *loc = matchLocation(req.path);
    std::cout << "[DEBUG] matchLocation('" << req.path << "') → "
          << (loc ? loc->path : "NULL") << "\n";


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
            HttpResponse res = makeErrorResponse(405, "Method Not Allowed");
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

    // DELETE handling
    if (req.method == "DELETE")
    {
        std::cout << "[DEBUG] = Entering DELETE handling" << std::endl;
        return handleDelete(fsPath);
    }

    // CGI always has priority (GET or POST)
    if (loc && !loc->cgi_path.empty() && !loc->cgi_extension.empty()) {
        std::cout << "[DEBUG] Entering CGI handling\n";

        CgiHandler cgi;
        std::string output = cgi.execute(fsPath, req, loc->cgi_path);

        return parseCgiResponse(output);
    }

    // POST upload handling (only if upload_store is set)
    if (req.method == "POST")
    {
        std::cout << "[DEBUG] Entering POST upload handling\n";

        if (!loc || loc->upload_store.empty())
        {
            std::cout << "[DEBUG] POST rejected: upload_store not set\n";
            return makeErrorResponse(403, "POST not allowed here (upload_store not set)");
        }
        return handlePost(req, loc);
    }
    // 5. Stat the path
    std::cout << "[DEBUG] fsPath1 = '" << fsPath << "'" << std::endl;
    struct stat st;
    if (stat(fsPath.c_str(), &st) < 0)
    {
        // stat falló → 404
        return makeErrorResponse(404, "Not Found");
    }
    // AHORA sí: si es directorio
    if (S_ISDIR(st.st_mode))
    {
        std::cout << "[DEBUG] st is a directory " << std::endl;
        bool autoindex = loc ? loc->autoindex : _config.autoindex;
        std::string index = (loc && !loc->index.empty()) ? loc->index : _config.index;

        return serveDirectory(fsPath, path, index, autoindex);
    }
    std::cout << "[DEBUG] st_mode = " << st.st_mode << " isDir = " << S_ISDIR(st.st_mode) << std::endl;
    
    // 6. Directory?
    if (S_ISDIR(st.st_mode))
    {
        std::cout << "Entered directory if" << std::endl;
        bool autoindex = loc ? loc->autoindex : _config.autoindex;
        std::string index = (loc && !loc->index.empty()) ? loc->index : _config.index;

        return serveDirectory(fsPath, path, index, autoindex);
    }

    // --- CGI detection ---
    if (loc && !loc->cgi_extension.empty() && !loc->cgi_path.empty()) {
        // Check extension
        size_t dot = fsPath.find_last_of('.');
        std::string ext = (dot != std::string::npos) ? fsPath.substr(dot) : "";

        if (ext == loc->cgi_extension) {
            std::cout << "[DEBUG] CGI detected for: " << fsPath << std::endl;

            CgiHandler cgi;
            std::string rawOutput = cgi.execute(fsPath, req, loc->cgi_path);

            return parseCgiResponse(rawOutput);
        }
    }
    // 7. File
    return serveFile(fsPath);
}


HttpResponse Router::serveFile(const std::string &path)
{
    HttpResponse res;

    if (!FileUtils::exists(path))
    {
        return makeErrorResponse(404, "Not Found");
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
    return makeErrorResponse(403, "Directory listing forbidden");
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

HttpResponse Router::handleDelete(const std::string &fsPath)
{
    HttpResponse res;
    struct stat st;

    if (stat(fsPath.c_str(), &st) < 0)
    {
        return makeErrorResponse(404, "Not Found");
    }

    if (S_ISDIR(st.st_mode))
    {
        return makeErrorResponse(403, "Forbidden");
    }

    if (std::remove(fsPath.c_str()) != 0)
    {
        return makeErrorResponse(500, "Internal Server Error");
    }

    // Success: 204 No Content
    res.status_code = 204;
    res.headers["Content-Length"] = "0";
    return res;
}

HttpResponse Router::handlePost(const HttpRequest &req, const LocationConfig *loc)
{
    HttpResponse res;

    // 1. upload_store must be configured
    if (loc->upload_store.empty())
    {
        return makeErrorResponse(403, "Forbidden (upload_store not set)");
    }

    // 2. Ensure upload directory exists
    std::string dir = loc->upload_store;
    if (dir[dir.size() - 1] != '/')
        dir += "/";

    // 3. Generate a filename (simple version)
    // Example: upload_1700000000.txt
    std::string filename = "upload_" + StringUtils::toString(time(NULL)) + ".txt";
    std::string fullpath = dir + filename;

    // 4. Write body to file
    FILE *f = fopen(fullpath.c_str(), "wb");
    if (!f)
    {
        return makeErrorResponse(500, "Internal Server Error (cannot open file)");
    }

    fwrite(req.body.c_str(), 1, req.body.size(), f);
    fclose(f);

    // 5. Return 201 Created
    res.status_code = 201;
    res.headers["Content-Length"] = "0";
    res.headers["Location"] = filename; // relative path
    return res;
}

HttpResponse Router::parseCgiResponse(const std::string &raw)
{
    HttpResponse res;

    size_t pos = raw.find("\r\n\r\n");
    size_t delimiterLen = 4;

    if (pos == std::string::npos)
    {
        pos = raw.find("\n\n");
        delimiterLen = 2;
    }

    if (pos == std::string::npos)
    {
        return makeErrorResponse(500, "Invalid CGI output");
    }

    std::string headerPart = raw.substr(0, pos);
    std::string bodyPart = raw.substr(pos + delimiterLen);


    std::istringstream hs(headerPart);
    std::string line;

    while (std::getline(hs, line)) {
        if (line.size() && line[line.size()-1] == '\r')
            line.erase(line.size()-1);

        if (line.find("Status:") == 0) {
            int code = atoi(line.substr(7).c_str());
            res.status_code = code;
        }
        else {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string val = line.substr(colon + 1);
                if (val.size() && val[0] == ' ')
                    val.erase(0, 1);
                res.headers[key] = val;
            }
        }
    }

    res.body = bodyPart;

    if (res.headers.find("Content-Length") == res.headers.end())
        res.headers["Content-Length"] = StringUtils::toString(res.body.size());

    if (res.headers.find("Content-Type") == res.headers.end())
        res.headers["Content-Type"] = "text/html";

    return res;
}

HttpResponse Router::generateAutoindexResponse(const std::string &urlPath, const std::string &fsPath)
{
    DIR *dir = opendir(fsPath.c_str());
    if (!dir)
        return makeErrorResponse(500, "Cannot open directory");

    std::string html;
    html += "<html><body>";
    html += "<h1>Index of " + urlPath + "</h1><ul>";

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        if (name == ".")
            continue;

        std::string full = fsPath + "/" + name;

        struct stat st;
        stat(full.c_str(), &st);

        // Add slash for directories
        if (S_ISDIR(st.st_mode))
            name += "/";

        html += "<li><a href=\"" + name + "\">" + name + "</a></li>";
    }

    html += "</ul></body></html>";
    closedir(dir);

    HttpResponse res;
    res.status_code = 200;
    res.body = html;
    res.headers["Content-Type"] = "text/html";
    res.headers["Content-Length"] = StringUtils::toString(html.size());
    return res;
}

// HttpResponse Router::makeErrorResponse(int code, const std::string &message)
// {
//     HttpResponse res;
//     res.status_code = code;

//     // 1. Check if there is a custom error page for this code
//     if (_config.error_pages.count(code))
//     {
//         std::string path = _config.root + _config.error_pages.at(code);

//         // Try to serve the custom file
//         struct stat st;
//         if (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode))
//         {
//             return serveFile(path);
//         }
//     }

//     // 2. Default error page (simple HTML)
//     std::string html;
//     html += "<html><body>";
//     html += "<h1>" + StringUtils::toString(code) + " " + message + "</h1>";
//     html += "</body></html>";

//     res.body = html;
//     res.headers["Content-Type"] = "text/html";
//     res.headers["Content-Length"] = StringUtils::toString(html.size());

//     return res;
// }

HttpResponse Router::makeErrorResponse(int code, const std::string &msg)
{
    HttpResponse res;
    res.status_code = code;

    // 1. ¿Hay página personalizada para este código?
    if (_config.error_pages.count(code))
    {
        std::string rel = _config.error_pages.at(code);   // ej: "/errors/404.html"
        std::string full = _config.root + rel;            // ej: "./www/errors/404.html"

        // 2. Intentar leer el archivo
        if (FileUtils::exists(full))
        {
            std::string body = FileUtils::readFile(full);
            res.body = body;
            std::string ext = FileUtils::getExtension(full);
            res.headers["Content-Type"] = MimeTypes::get(ext);
            res.headers["Content-Length"] = StringUtils::toString(body.size());
            return res;
        }
    }

    // 3. Si no hay página personalizada o no existe → fallback genérico
    std::string body = "<html><body><h1>" +
                       StringUtils::toString(code) + " " + msg +
                       "</h1></body></html>";

    res.body = body;
    res.headers["Content-Type"] = "text/html";
    res.headers["Content-Length"] = StringUtils::toString(body.size());
    return res;
}