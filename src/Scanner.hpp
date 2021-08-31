#pragma once

#include "Token.hpp"

namespace cloxx {

class ErrorReporter;
class SourceReader;

class Scanner {
public:
    explicit Scanner(ErrorReporter* errorReporter, SourceReader* sourceReader);

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

    ErrorReporter* const _errorReporter;
    SourceReader* const _sourceReader;

    char _currentChar = '\0';
    char _nextChar = '\0';
    std::string _lexeme;

    size_t _line = 1;
};
} // namespace cloxx
