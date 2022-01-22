#include "Util.hpp"

#include <cctype>
#include <string>
#include <sstream>
#include <iostream>

bool isTerm(const std::string& str)
{
    return !isNonTerm(str);
}

bool isNonTerm(const std::string& str)
{
    return std::isupper(str[0]);
}

void tokenizeBySpaces(const std::string& str, std::vector<std::string>& out)
{
    std::istringstream sin(str);
    std::string token;

    while (sin >> token) {
        out.push_back(token);
    }
}
