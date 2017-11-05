//
// Created by Daniel S on 01.11.2017.
// Just a util file for the examples
//

#ifndef SOCKET_UTIL_H
#define SOCKET_UTIL_H

#include <vector>
#include <string>

std::vector<std::string> split(std::string str, std::string delim){
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

#endif //SOCKET_UTIL_H
