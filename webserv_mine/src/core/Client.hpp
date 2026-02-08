/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:48:24 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/08 16:47:47 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
	public:
		int fd;
		std::string readBuffer;
		std::string writeBuffer;
		bool wantWrite;

		Client(int fd);
		~Client();

};

#endif