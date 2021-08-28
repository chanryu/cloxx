#include "Scanner.hpp"

#include "Lox.hpp"

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

std::map<std::string, Token::Type> const Scanner::_keywords = {
    {"and", Token::AND},   {"class", Token::CLASS}, {"else", Token::ELSE},     {"false", Token::FALSE},
    {"for", Token::FOR},   {"fun", Token::FUN},     {"if", Token::IF},         {"nil", Token::NIL},
    {"or", Token::OR},     {"print", Token::PRINT}, {"return", Token::RETURN}, {"super", Token::SUPER},
    {"this", Token::THIS}, {"true", Token::TRUE},   {"var", Token::VAR},       {"while", Token::WHILE},
};

Scanner::Scanner(Lox* const lox) : _lox{lox}
{
    readSource();
}

bool Scanner::isAtEnd()
{
    if (_current < _source.length()) {
        return false;
    }

    if (_sourceEnded) {
        return true;
    }

    readSource();
    return _current >= _source.length();
}

Token Scanner::scanToken()
{
    using T = Token;

    while (!isAtEnd()) {
        _start = _current;

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
            break;
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
            _lox->error(_line, "Unexpected character.");
            break;
        }
    }

    return {Token::END_OF_FILE, "", nullptr, _line};
}

char Scanner::advance()
{
    return _source[_current++];
}

Token Scanner::makeToken(Token::Type type)
{
    return makeToken(type, nullptr);
}

Token Scanner::makeToken(Token::Type type, std::shared_ptr<LoxObject> const& literal)
{
    auto lexeme = _source.substr(_start, _current - _start);
    return {type, std::move(lexeme), literal, _line};
}

bool Scanner::match(char expected)
{
    if (isAtEnd())
        return false;

    if (_source[_current] != expected)
        return false;

    _current++;
    return true;
}

char Scanner::peek()
{
    if (isAtEnd())
        return '\0';

    return _source[_current];
}

char Scanner::peekNext()
{
    while (true) {
        if (_current + 1 < _source.length()) {
            return _source[_current + 1];
        }

        if (_sourceEnded) {
            return '\0';
        }

        readSource();
    }
}

Token Scanner::string()
{
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n')
            _line++;
        advance();
    }

    if (isAtEnd()) {
        _lox->error(_line, "Unterminated string.");
        return {Token::END_OF_FILE, "", nullptr, _line};
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    auto value = _source.substr(_start + 1, _current - _start - 2);
    return makeToken(Token::STRING, std::make_shared<LoxString>(std::move(value)));
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

    auto value = std::stod(_source.substr(_start, _current - _start));
    return makeToken(Token::NUMBER, std::make_shared<LoxNumber>(value));
}

Token Scanner::identifier()
{
    while (isAlphaNumeric(peek())) {
        advance();
    }

    auto text = _source.substr(_start, _current - _start);
    if (auto it = _keywords.find(text); it != _keywords.end()) {
        return makeToken(it->second);
    }

    return makeToken(Token::IDENTIFIER);
}

void Scanner::readSource()
{
    if (!_sourceEnded) {

        // TODO: _source.erase(0, _start)

        std::string line;
        _sourceEnded = !_lox->readLine(line);
        _source.append(line.begin(), line.end());
    }
}

} // namespace cloxx
