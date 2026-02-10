/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:05:01 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/10 14:46:25 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "../utils/StringUtils.hpp"

ConfigParser::ConfigParser()
    : _pos(0)
{}

ConfigParser::~ConfigParser()
{}

const Token &ConfigParser::peek()
{
    if (_pos >= _tokens.size())
        throw std::runtime_error("Unexpected end of tokens");
    return _tokens[_pos];
}

const Token &ConfigParser::get()
{
    if (_pos >= _tokens.size())
        throw std::runtime_error("Unexpected end of tokens");
    return _tokens[_pos++];
}

bool ConfigParser::match(TokenType type)
{
    if (peek().type == type)
    {
        get();
        return true;
    }
    return false;
}

void ConfigParser::expect(TokenType type, const std::string &msg)
{
    if (peek().type != type)
        throw std::runtime_error(msg + " at line " + StringUtils::toString(peek().line));
    get();
}

std::vector<ServerConfig> ConfigParser::parse(const std::string &path)
{
    Tokenizer tokenizer(path);
    _tokens = tokenizer.tokenize();
    _pos = 0;

    std::vector<ServerConfig> servers;

    while (peek().type != TOKEN_END) {
        if (peek().type != TOKEN_IDENT || peek().value != "server")
            throw std::runtime_error("Expected 'server' at line " + StringUtils::toString(peek().line));

        get(); // consume 'server'
        expect(TOKEN_LBRACE, "Expected '{' after server");

        servers.push_back(parseServerBlock());
    }

    return servers;
}

ServerConfig ConfigParser::parseServerBlock()
{
    ServerConfig srv;
    srv.client_max_body_size = 0;

    while (peek().type != TOKEN_RBRACE)
        parseServerDirective(srv);

    expect(TOKEN_RBRACE, "Expected '}' at end of server block");
    return srv;
}

void ConfigParser::parseServerDirective(ServerConfig &srv)
{
    const Token &tok = peek();

    if (tok.type != TOKEN_IDENT)
        throw std::runtime_error("Expected directive at line " + StringUtils::toString(tok.line));

    std::string key = tok.value;
    get(); // consume directive name

    if (key == "listen")
    {
        srv.listen.push_back(get().value);
        expect(TOKEN_SEMICOLON, "Expected ';' after listen");
    }
    else if (key == "error_page")
    {
        int code = StringUtils::toInt(get().value);
        std::string path = get().value;
        srv.error_pages[code] = path;
        expect(TOKEN_SEMICOLON, "Expected ';' after error_page");
    }
    else if (key == "client_max_body_size")
    {
        srv.client_max_body_size = StringUtils::toSizeT(get().value);
        expect(TOKEN_SEMICOLON, "Expected ';' after client_max_body_size");
    }
    else if (key == "location")
    {
        LocationConfig loc = parseLocationBlock();
        srv.locations.push_back(loc);
    }
    else if (key == "server_name")
    {
        srv.server_name = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after server_name");
    }
    else if (key == "root")
    {
        srv.root = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after root");
    }
    else if (key == "index")
    {
        srv.index = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after index");
    }
    else if (key == "autoindex")
    {
        std::string val = get().value;
        srv.autoindex = (val == "on");
        expect(TOKEN_SEMICOLON, "Expected ';' after autoindex");
    }
    else
        throw std::runtime_error("Unknown directive: " + key);
}

LocationConfig ConfigParser::parseLocationBlock()
{
    LocationConfig loc;

    loc.path = get().value; // location /path
    expect(TOKEN_LBRACE, "Expected '{' after location path");

    while (peek().type != TOKEN_RBRACE)
        parseLocationDirective(loc);

    expect(TOKEN_RBRACE, "Expected '}' at end of location block");
    return loc;
}

void ConfigParser::parseLocationDirective(LocationConfig &loc)
{
    const Token &tok = peek();

    if (tok.type != TOKEN_IDENT)
        throw std::runtime_error("Expected directive inside location");

    std::string key = tok.value;
    get();

    if (key == "root")
    {
        loc.root = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after root");
    }
    else if (key == "autoindex")
    {
        loc.autoindex = (get().value == "on");
        expect(TOKEN_SEMICOLON, "Expected ';' after autoindex");
    }
    else if (key == "allow_methods")
    {
        // Example: allow_methods GET POST DELETE;
        while (peek().type == TOKEN_IDENT)
            loc.methods.push_back(get().value);

        expect(TOKEN_SEMICOLON, "Expected ';' after allow_methods");
    }
    else
        throw std::runtime_error("Unknown location directive: " + key);
}






