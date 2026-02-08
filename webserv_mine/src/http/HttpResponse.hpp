/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:52:55 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 13:53:08 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <map>

class HttpResponse {
	public:
		int status_code;
		std::map<std::string, std::string> headers;
		std::string body;

		HttpResponse();
		std::string serialize() const;
};

#endif