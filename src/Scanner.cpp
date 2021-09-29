#include "Scanner.hpp"

#include <map>
#include <optional>

#include "Assert.hpp"
#include "ErrorReporter.hpp"
#include "ScriptReader.hpp"

namespace cloxx {

namespace {
bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}
bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
bool isAlphaNumeric(char c)
{
    return isAlpha(c) || isDigit(c);
}
} // namespace

Scanner::Scanner(ErrorReporter* errorReporter, ScriptReader* scriptReader)
    : _errorReporter{errorReporter}, _scriptReader{scriptReader}
{
    _filePath = std::make_shared<std::string>(_scriptReader->filePath());
    _currentChar = _scriptReader->readChar();
}

bool Scanner::isAtEnd()
{
    return _currentChar == '\0';
}

Token Scanner::scanToken()
{
    using T = Token;

    while (!isAtEnd()) {
        char c = advance();
        switch (c) {
        case '(':
            return makeToken(T::LEFT_PAREN);
        case ')':
            return makeToken(T::RIGHT_PAREN);
        case '{':
            return makeToken(T::LEFT_BRACE);
        case '}':
            return makeToken(T::RIGHT_BRACE);
        case ',':
            return makeToken(T::COMMA);
        case '.':
            return makeToken(T::DOT);
        case '-':
            return makeToken(T::MINUS);
        case '+':
            return makeToken(T::PLUS);
        case ';':
            return makeToken(T::SEMICOLON);
        case '*':
            return makeToken(T::STAR);
        case '!':
            return makeToken(match('=') ? T::BANG_EQUAL : T::BANG);
        case '=':
            return makeToken(match('=') ? T::EQUAL_EQUAL : T::EQUAL);
        case '<':
            return makeToken(match('=') ? T::LESS_EQUAL : T::LESS);
        case '>':
            return makeToken(match('=') ? T::GREATER_EQUAL : T::GREATER);
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                break;
            }
            return makeToken(T::SLASH);

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            _line++;
            break;

        case '"':
            return string();

        default:
            if (isDigit(c)) {
                return number();
            }
            if (isAlpha(c)) {
                return identifier();
            }
            _errorReporter->syntaxError(*_filePath, _line, "Unexpected character.");
            break;
        }

        _lexeme.clear();
    }

    return makeToken(T::END_OF_FILE);
}

char Scanner::advance()
{
    LOX_ASSERT(_currentChar != '\0');

    _lexeme.push_back(_currentChar);

    peekNext();

    _currentChar = _nextChar;
    _nextChar = '\0';

    return _lexeme.back();
}

Token Scanner::makeToken(Token::Type type)
{
    std::string lexeme;
    lexeme.swap(_lexeme);
    return {type, std::move(lexeme), _filePath, _line};
}

bool Scanner::match(char expected)
{
    if (isAtEnd())
        return false;

    if (peek() != expected)
        return false;

    advance();
    return true;
}

char Scanner::peek()
{
    return _currentChar;
}

char Scanner::peekNext()
{
    if (_nextChar == '\0' && !_scriptReader->isAtEnd()) {
        _nextChar = _scriptReader->readChar();
    }

    return _nextChar;
}

Token Scanner::string()
{
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n')
            _line++;
        advance();
    }

    if (isAtEnd()) {
        _errorReporter->syntaxError(*_filePath, _line, "Unterminated string.");
        return makeToken(Token::END_OF_FILE);
    }

    // The closing ".
    advance();

    return makeToken(Token::STRING);
}

Token Scanner::number()
{
    while (isDigit(peek())) {
        advance();
    }

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) {
            advance();
        }
    }

    return makeToken(Token::NUMBER);
}

Token Scanner::identifier()
{
    while (isAlphaNumeric(peek())) {
        advance();
    }

    if (auto type = lookupKeyword(_lexeme)) {
        return makeToken(*type);
    }

    return makeToken(Token::IDENTIFIER);
}

} // namespace cloxx
