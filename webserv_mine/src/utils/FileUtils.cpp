/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:15:15 by aloiki            #+#    #+#             */
/*   Updated: 2026/03/01 15:33:29 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileUtils.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

bool FileUtils::exists(const std::string &path)
{
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}


std::string FileUtils::readFile(const std::string &path)
{
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file)
        return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string FileUtils::getExtension(const std::string &path)
{
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return "";
    return path.substr(dot);
}

std::string FileUtils::getLastModified(const std::string &path)
{
    struct stat st;
    if (stat(path.c_str(), &st) != 0)
        return "";

    char buf[100];
    struct tm tm = *gmtime(&st.st_mtime);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &tm);
    return std::string(buf);
}