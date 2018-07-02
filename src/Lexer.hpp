#ifndef LEXER_HPP
#define LEXER_HPP

#include "Foundation.hpp"
#include <regex>

class Lexer {
public:
    static std::vector<std::vector<std::string>> lexem;
    static void lex(std::string file);
};

#endif
