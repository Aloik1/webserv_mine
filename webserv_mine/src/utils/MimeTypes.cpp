/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypes.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:15:45 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 16:31:44 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MimeTypes.hpp"

std::string MimeTypes::get(const std::string &ext)
{
    if (ext == ".html") return "text/html";
    if (ext == ".css")  return "text/css";
    if (ext == ".js")   return "application/javascript";
    if (ext == ".png")  return "image/png";
    if (ext == ".jpg")  return "image/jpeg";
    if (ext == ".jpeg") return "image/jpeg";
    if (ext == ".gif")  return "image/gif";
    if (ext == ".txt")  return "text/plain";

    return "application/octet-stream";
}