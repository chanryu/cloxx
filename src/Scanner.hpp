#pragma once

#include <map>
#include <vector>

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
    Token makeToken(Token::Type type, std::shared_ptr<LoxObject> const& literal);

    void readSource();

    static std::map<std::string, Token::Type> const _keywords;

    Lox* const _lox;

    std::string _source;
    bool _sourceEnded = false;

    size_t _start = 0;
    size_t _current = 0;
    size_t _line = 1;
    std::vector<Token> _tokens;
};
} // namespace cloxx
