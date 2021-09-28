#pragma once

#include "Token.hpp"

namespace cloxx {

class ErrorReporter;
class ScriptReader;

class Scanner {
public:
    explicit Scanner(ErrorReporter* errorReporter, ScriptReader* sourceReader);

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
    ScriptReader* const _scriptReader;

    char _currentChar = '\0';
    char _nextChar = '\0';
    std::string _lexeme;

    std::shared_ptr<std::string> _filePath;
    size_t _line = 1;
};
} // namespace cloxx
