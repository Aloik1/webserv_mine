/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:49:49 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 14:32:17 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <vector>
#include "Tokenizer.hpp"
#include "ServerConfig.hpp"

class ConfigParser {
	public:
		ConfigParser();
		~ConfigParser();

		std::vector<ServerConfig> parse(const std::string &path);

	private:
		std::vector<Token> _tokens;
		size_t _pos;

		// Token helpers
		const Token &peek();
		const Token &get();
		bool match(TokenType type);
		void expect(TokenType type, const std::string &msg);

		// Parsing functions
		ServerConfig parseServerBlock();
		void parseServerDirective(ServerConfig &srv);

		LocationConfig parseLocationBlock();
		void parseLocationDirective(LocationConfig &loc);
};

#endif