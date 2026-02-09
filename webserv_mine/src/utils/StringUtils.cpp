/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:14:14 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 15:09:44 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StringUtils.hpp"
#include <sstream>

namespace StringUtils {
	std::vector<std::string> split(const std::string &s, char delim)
	{
		std::vector<std::string> result;
		std::stringstream ss(s);
		std::string item;

		while (std::getline(ss, item, delim))
		{
			result.push_back(item);
		}
		return result;
	}

	std::string trim(const std::string &s)
	{
		size_t start = s.find_first_not_of(" \t\r\n");
		size_t end = s.find_last_not_of(" \t\r\n");

		if (start == std::string::npos)
			return "";

		return s.substr(start, end - start + 1);
	}

	std::string toString(size_t n)
	{
	std::stringstream ss;
	ss << n;
	return ss.str();
	}

	int toInt(const std::string &s)
	{
	std::stringstream ss(s);
	int n;
	ss >> n;
	return n;
	}

	size_t toSizeT(const std::string &s)
	{
	std::stringstream ss(s);
	size_t n;
	ss >> n;
	return n;
	}
}

