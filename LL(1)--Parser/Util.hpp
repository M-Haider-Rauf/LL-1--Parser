#ifndef  UTIL_HPP
#define UTIL_HPP

#include <string>
#include <vector>

bool isTerm(const std::string& str);
bool isNonTerm(const std::string& str);

void tokenizeBySpaces(const std::string& str, std::vector<std::string>& out);

#endif // ! UTIL_HPP
