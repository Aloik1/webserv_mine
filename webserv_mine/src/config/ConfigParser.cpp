/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:05:01 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/28 17:03:16 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
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

    while (peek().type != TOKEN_END)
    {
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
    srv.client_max_body_size = 104857600; // 100MB default for tester
    srv.autoindex = false;
    srv.root = "./www";
    srv.index = "index.html";
    srv.server_name = "";

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
        // First token: number or number+suffix
        Token t1 = get();
        std::string val = t1.value;

        // Check if next token is a suffix (K/M/G)
        if (peek().type == TOKEN_IDENT &&
            (peek().value == "K" || peek().value == "k" ||
            peek().value == "M" || peek().value == "m" ||
            peek().value == "G" || peek().value == "g"))
        {
            Token suffix = get();
            val += suffix.value; // append suffix
        }

        // Now val is guaranteed to be like "100", "100M", "100K", etc.

        // Detect suffix
        size_t multiplier = 1;
        char last = val[val.size() - 1];

        if (last == 'K' || last == 'k')
        {
            multiplier = 1024;
            val = val.substr(0, val.size() - 1);
        }
        else if (last == 'M' || last == 'm')
        {
            multiplier = 1024 * 1024;
            val = val.substr(0, val.size() - 1);
        }
        else if (last == 'G' || last == 'g')
        {
            multiplier = 1024 * 1024 * 1024;
            val = val.substr(0, val.size() - 1);
        }

        srv.client_max_body_size = StringUtils::toSizeT(val) * multiplier;

        expect(TOKEN_SEMICOLON, "Expected ';' after client_max_body_size");
    }

    else if (key == "location")
    {
        LocationConfig loc = parseLocationBlock();
        srv.locations.push_back(loc);
    }
    else if (key == "server_name")
    {
        if (peek().type != TOKEN_SEMICOLON)
            srv.server_name = get().value;
        while (peek().type != TOKEN_SEMICOLON)
            get();
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
    else if (key == "cgi-bin")
    {
        srv.cgi_bin = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after cgi-bin");
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
        loc.is_alias = true; // For this project, root in location is treated as alias
        expect(TOKEN_SEMICOLON, "Expected ';' after root");
    }
    else if (key == "alias")
    {
        loc.root = get().value;
        loc.is_alias = true;
        expect(TOKEN_SEMICOLON, "Expected ';' after alias");
    }
    else if (key == "autoindex")
    {
        loc.autoindex = (get().value == "on");
        expect(TOKEN_SEMICOLON, "Expected ';' after autoindex");
    }
    else if (key == "index")
    {
        loc.index = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after index");
    }
    else if (key == "allow_methods" || key == "limit_except")
    {
        while (peek().type == TOKEN_IDENT)
            loc.methods.push_back(get().value);

        expect(TOKEN_SEMICOLON, "Expected ';' after methods");
    }
    else if (key == "upload_store" || key == "upload")
    {
        loc.upload_store = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after upload");
    }
    else if (key == "cgi_extension")
    {
        loc.cgi_extension = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after cgi_extension");
    }
    else if (key == "cgi_path")
    {
        loc.cgi_path = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after cgi_path");
    }
    else if (key == "cgi")
    {
        // cgi extension handler_path
        loc.cgi_extension = get().value;
        loc.cgi_path = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after cgi");
    }
    else if (key == "auth_basic")
    {
        loc.auth_basic = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after auth_basic");
    }
    else if (key == "auth_basic_user_file")
    {
        loc.auth_basic_user_file = get().value;
        expect(TOKEN_SEMICOLON, "Expected ';' after auth_basic_user_file");
    }
    else if (key == "cgi-bin")
    {
        // ignore in server block too or add to server config
        get();
        expect(TOKEN_SEMICOLON, "Expected ';' after cgi-bin");
    }
    else if (key == "client_max_body_size")
    {
        Token t1 = get();
        std::string val = t1.value;
        if (peek().type == TOKEN_IDENT && (peek().value == "K" || peek().value == "k" || peek().value == "M" || peek().value == "m" || peek().value == "G" || peek().value == "g"))
        {
            Token suffix = get();
            val += suffix.value;
        }
        size_t multiplier = 1;
        char last = val[val.size() - 1];
        if (last == 'K' || last == 'k')
        {
            multiplier = 1024;
            val = val.substr(0, val.size() - 1);
        }
        else if (last == 'M' || last == 'm')
        {
            multiplier = 1024 * 1024;
            val = val.substr(0, val.size() - 1);
        }
        else if (last == 'G' || last == 'g')
        {
            multiplier = 1024 * 1024 * 1024;
            val = val.substr(0, val.size() - 1);
        }
        loc.client_max_body_size = StringUtils::toSizeT(val) * multiplier;
        expect(TOKEN_SEMICOLON, "Expected ';' after client_max_body_size");
    }
    else
        throw std::runtime_error("Unknown location directive: " + key);
}






