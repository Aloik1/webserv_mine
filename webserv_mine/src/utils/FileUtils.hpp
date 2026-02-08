/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:56:12 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 13:56:14 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <string>

namespace FileUtils {
    bool exists(const std::string &path);
    std::string readFile(const std::string &path);
}

#endif