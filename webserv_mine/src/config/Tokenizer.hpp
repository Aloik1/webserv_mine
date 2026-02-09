/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 14:03:10 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 14:03:13 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <string>
#include <vector>
#include <stdexcept>

enum TokenType 
{
    TOKEN_IDENT,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_LBRACE,   // {
    TOKEN_RBRACE,   // }
    TOKEN_SEMICOLON,// ;
    TOKEN_END
};

struct Token 
{
    TokenType   type;
    std::string value;
    int         line;
    int         col;
};

class Tokenizer
{
	public:
		Tokenizer(const std::string &path);
		std::vector<Token> tokenize();

	private:
		std::string _input;
		size_t      _pos;
		int         _line;
		int         _col;

		char peek() const;
		char get();
		bool eof() const;

		void skipWhitespace();
		void skipComment();

		Token readIdentOrKeyword();
		Token readNumber();
		Token readString();
};