/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:55:34 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 14:46:22 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>
#include <vector>

namespace StringUtils {
    std::vector<std::string> split(const std::string &s, char delim);
    std::string trim(const std::string &s);
    std::string toString(size_t n);
    int toInt(const std::string &s);
    size_t toSizeT(const std::string &s);
}

#endif