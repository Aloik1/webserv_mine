/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypes.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:56:46 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 13:56:48 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MIME_TYPES_HPP
#define MIME_TYPES_HPP

#include <string>

namespace MimeTypes {
    std::string get(const std::string &ext);
}

#endif