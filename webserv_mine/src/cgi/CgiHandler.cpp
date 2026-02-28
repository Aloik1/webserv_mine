/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloiki <aloiki@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 14:09:11 by aloiki            #+#    #+#             */
/*   Updated: 2026/02/28 16:09:27 by aloiki           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "../utils/StringUtils.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <cstdlib>


CgiHandler::CgiHandler() {}

std::string CgiHandler::execute(const std::string &script, const HttpRequest &req, const std::string &interpreter)
{
    int inPipe[2];
    int outPipe[2];

    pipe(inPipe);
    pipe(outPipe);

    pid_t pid = fork();

    if (pid == 0) {
        // CHILD
        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);

        close(inPipe[1]);
        close(outPipe[0]);

        // Build argv
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(interpreter.c_str()));
        argv.push_back(const_cast<char*>(script.c_str()));
        argv.push_back(NULL);

        // Build environment
        std::vector<std::string> env;
        env.push_back("REQUEST_METHOD=" + req.method);
        env.push_back("CONTENT_LENGTH=" + StringUtils::toString(req.body.size()));
        if (req.headers.count("Content-Type"))
            env.push_back("CONTENT_TYPE=" + req.headers.at("Content-Type"));
        env.push_back("SCRIPT_FILENAME=" + script);
        env.push_back("SCRIPT_NAME=" + script);
        env.push_back("GATEWAY_INTERFACE=CGI/1.1");
        env.push_back("SERVER_PROTOCOL=HTTP/1.1");

        std::vector<char*> envp;
        for (size_t i = 0; i < env.size(); i++)
            envp.push_back(const_cast<char*>(env[i].c_str()));
        envp.push_back(NULL);

        execve(interpreter.c_str(), &argv[0], &envp[0]);
        exit(1);
    }

    // PARENT
    close(inPipe[0]);
    close(outPipe[1]);

    // Write body to CGI
    write(inPipe[1], req.body.c_str(), req.body.size());
    close(inPipe[1]);

    // Read CGI output
    char buffer[4096];
    std::string output;
    ssize_t n;

    while ((n = read(outPipe[0], buffer, sizeof(buffer))) > 0)
        output.append(buffer, n);

    close(outPipe[0]);
    waitpid(pid, NULL, 0);
    
    std::cout << "\n===== CGI RAW OUTPUT BEGIN =====\n";
    std::cout << output << "\n";
    std::cout << "===== CGI RAW OUTPUT END =====\n\n";

    return output;
}