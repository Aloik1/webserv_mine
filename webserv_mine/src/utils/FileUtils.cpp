/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:15:15 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 16:11:57 by aloiki           ###   ########.fr       */
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
