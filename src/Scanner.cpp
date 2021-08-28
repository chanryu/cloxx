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

std::vector<Token> Scanner::scanTokens()
{
    while (!isAtEnd()) {
        // We are at the beginning of the next lexeme.
        _start = _current;
        scanToken();

        if (_tokens.size() == 10) {
            break;
        }
    }

    if (isAtEnd()) {
        _tokens.emplace_back(Token::END_OF_FILE, "", nullptr, _line);
    }

    std::vector<Token> tokens;
    tokens.swap(_tokens);
    return tokens;
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

void Scanner::scanToken()
{
    using T = Token;

    char c = advance();
    switch (c) {
    case '(':
        addToken(T::LEFT_PAREN);
        break;
    case ')':
        addToken(T::RIGHT_PAREN);
        break;
    case '{':
        addToken(T::LEFT_BRACE);
        break;
    case '}':
        addToken(T::RIGHT_BRACE);
        break;
    case ',':
        addToken(T::COMMA);
        break;
    case '.':
        addToken(T::DOT);
        break;
    case '-':
        addToken(T::MINUS);
        break;
    case '+':
        addToken(T::PLUS);
        break;
    case ';':
        addToken(T::SEMICOLON);
        break;
    case '*':
        addToken(T::STAR);
        break;
    case '!':
        addToken(match('=') ? T::BANG_EQUAL : T::BANG);
        break;
    case '=':
        addToken(match('=') ? T::EQUAL_EQUAL : T::EQUAL);
        break;
    case '<':
        addToken(match('=') ? T::LESS_EQUAL : T::LESS);
        break;
    case '>':
        addToken(match('=') ? T::GREATER_EQUAL : T::GREATER);
        break;
    case '/':
        if (match('/')) {
            // A comment goes until the end of the line.
            while (peek() != '\n' && !isAtEnd())
                advance();
        }
        else {
            addToken(T::SLASH);
        }
        break;

    case ' ':
    case '\r':
    case '\t':
        // Ignore whitespace.
        break;

    case '\n':
        _line++;
        break;

    case '"':
        string();
        break;

    default:
        if (isDigit(c)) {
            number();
        }
        else if (isAlpha(c)) {
            identifier();
        }
        else {
            _lox->error(_line, "Unexpected character.");
        }
        break;
    }
}

char Scanner::advance()
{
    return _source[_current++];
}

void Scanner::addToken(Token::Type type)
{
    addToken(type, nullptr);
}

void Scanner::addToken(Token::Type type, std::shared_ptr<LoxObject> const& literal)
{
    auto lexeme = _source.substr(_start, _current - _start);
    _tokens.emplace_back(type, std::move(lexeme), literal, _line);
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

void Scanner::string()
{
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n')
            _line++;
        advance();
    }

    if (isAtEnd()) {
        _lox->error(_line, "Unterminated string.");
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    auto value = _source.substr(_start + 1, _current - _start - 2);
    addToken(Token::STRING, std::make_shared<LoxString>(std::move(value)));
}

void Scanner::number()
{
    while (isDigit(peek())) {
        advance();
    }

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek()))
            advance();
    }

    auto value = std::stod(_source.substr(_start, _current - _start));
    addToken(Token::NUMBER, std::make_shared<LoxNumber>(value));
}

void Scanner::identifier()
{
    while (isAlphaNumeric(peek())) {
        advance();
    }

    auto text = _source.substr(_start, _current - _start);
    if (auto it = _keywords.find(text); it != _keywords.end()) {
        addToken(it->second);
    }
    else {
        addToken(Token::IDENTIFIER);
    }
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
