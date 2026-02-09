/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 13:57:02 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/09 14:43:53 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core/Server.hpp"

// int main()//int argc, char **argv) 
// {	
// 	Server server;
// 	server.start();
// 	return 0;
// }

// #include "config/Tokenizer.hpp"
// #include <iostream>

// int main() {
//     Tokenizer t("webserv.conf");
//     std::vector<Token> tokens = t.tokenize();

//     for (size_t i = 0; i < tokens.size(); ++i) {
//         std::cout << "Token " << i
//                   << " type=" << tokens[i].type
//                   << " value='" << tokens[i].value
//                   << "'\n";
//     }
// }

#include "config/ConfigParser.hpp"
#include <iostream>

int main() {
    try {
        ConfigParser parser;
        std::vector<ServerConfig> servers = parser.parse("webserv.conf");

        std::cout << "Parsed " << servers.size() << " server blocks\n";

        for (size_t i = 0; i < servers.size(); i++) {
            std::cout << "Server " << i << ":\n";
            for (size_t j = 0; j < servers[i].listen.size(); j++)
                std::cout << "  listen: " << servers[i].listen[j] << "\n";

            for (size_t j = 0; j < servers[i].locations.size(); j++)
                std::cout << "  location: " << servers[i].locations[j].path << "\n";
        }
    }
    catch (const std::exception &e) {
        std::cerr << "Config error: " << e.what() << "\n";
    }
}

