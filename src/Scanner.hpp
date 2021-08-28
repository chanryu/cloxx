#pragma once

#include <map>
#include <vector>

#include "Token.hpp"

namespace cloxx {

class Lox;

class Scanner {
public:
    explicit Scanner(Lox* lox);

    std::vector<Token> scanTokens();

private:
    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(Token::Type type);
    void addToken(Token::Type type, std::shared_ptr<LoxObject> const& literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();

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
