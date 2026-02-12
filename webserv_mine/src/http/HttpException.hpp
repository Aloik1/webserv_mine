/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 16:01:09 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/12 16:01:25 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

class HttpException {
public:
    int code;
    std::string message;

    HttpException(int c, const std::string &msg)
        : code(c), message(msg) {}
};