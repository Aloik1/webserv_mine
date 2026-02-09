/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 14:03:15 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 14:16:24 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Tokenizer.hpp"
#include <fstream>
#include <cctype>

Tokenizer::Tokenizer(const std::string &path)
	: _pos(0), _line(1), _col(1)
{
	std::ifstream ifs(path.c_str());
	if (!ifs)
		throw std::runtime_error("Failed to open config file: " + path);

	std::string line;
	while (std::getline(ifs, line))
	{
		_input += line;
		_input += '\n';
	}
}

char Tokenizer::peek() const
{
	if (_pos >= _input.size())
		return '\0';
	return _input[_pos];
}

char Tokenizer::get()
{
	if (_pos >= _input.size())
		return '\0';
	char c = _input[_pos++];
	if (c == '\n')
	{
		_line++;
		_col = 1;
	} 
	else
		_col++;
	return c;
}

bool Tokenizer::eof() const
{
	return _pos >= _input.size();
}

void Tokenizer::skipWhitespace()
{
	while (!eof())
	{
		char c = peek();
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			get();
		else
			break;
	}
}

void Tokenizer::skipComment()
{
	if (peek() == '#')
	{
		while (!eof() && get() != '\n')
			;
	}
}


Token Tokenizer::readIdentOrKeyword()
{
	Token tok;
	tok.type = TOKEN_IDENT;
	tok.line = _line;
	tok.col  = _col;

	std::string s;
	while (!eof() && (std::isalnum(peek()) || peek() == '_' || peek() == '.'))
		s += get();
	tok.value = s;
	return tok;
}

Token Tokenizer::readNumber()
{
	Token tok;
	tok.type = TOKEN_NUMBER;
	tok.line = _line;
	tok.col  = _col;

	std::string s;
	while (!eof() && std::isdigit(peek()))
		s += get();
	tok.value = s;
	return tok;
}

Token Tokenizer::readString()
{
	Token tok;
	tok.type = TOKEN_STRING;
	tok.line = _line;
	tok.col  = _col;

	std::string s;
	while (!eof())
	{
		char c = peek();
		if (std::isspace(c) || c == ';' || c == '{' || c == '}')
			break;
		s += get();
	}
	tok.value = s;
	return tok;
}

std::vector<Token> Tokenizer::tokenize() {
	std::vector<Token> tokens;

	while (!eof())
	{
		skipWhitespace();
		skipComment();
		skipWhitespace();

		if (eof())
			break;
		char c = peek();
		if (std::isalpha(c) || c == '_')
			tokens.push_back(readIdentOrKeyword());
		else if (std::isdigit(c))
			tokens.push_back(readNumber());
		else if (c == '{')
		{
			Token t;
			t.type = TOKEN_LBRACE;
			t.value = "{";
			t.line = _line;
			t.col = _col;
			get();
			tokens.push_back(t);
		}
		else if (c == '}')
		{
			Token t;
			t.type = TOKEN_RBRACE;
			t.value = "}";
			t.line = _line;
			t.col = _col;
			get();
			tokens.push_back(t);
		}
		else if (c == ';')
		{
			Token t;
			t.type = TOKEN_SEMICOLON;
			t.value = ";";
			t.line = _line;
			t.col = _col;
			get();
			tokens.push_back(t);
		}
		else
		{
				// treat as STRING (paths like ./www, /test, etc.)
				tokens.push_back(readString());
		}
	}

	Token end;
	end.type = TOKEN_END;
	end.value = "";
	end.line = _line;
	end.col  = _col;
	tokens.push_back(end);

	return tokens;
}






