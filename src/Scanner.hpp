#pragma once

#include "Token.hpp"

namespace cloxx {

class Lox;

class Scanner {
public:
    explicit Scanner(Lox* lox);

    Token scanToken();

private:
    bool isAtEnd();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();

    Token string();
    Token number();
    Token identifier();
    Token makeToken(Token::Type type);

    void readSource();

    Lox* const _lox;

    std::string _source;
    bool _sourceEnded = false;

    size_t _start = 0;
    size_t _current = 0;
    size_t _line = 1;
};
} // namespace cloxx
