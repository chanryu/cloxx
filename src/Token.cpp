#include "Token.hpp"

#include <sstream>

namespace cloxx {

#ifndef NDEBUG
namespace {
#define LOX_TOKEN_NAME_CASE(name)                                                                                      \
    case Token::Type::name:                                                                                            \
        return #name

char const* getTokenName(Token::Type tokenType)
{
    switch (tokenType) {
        // Single-character tokens.
        LOX_TOKEN_NAME_CASE(LEFT_PAREN);
        LOX_TOKEN_NAME_CASE(RIGHT_PAREN);
        LOX_TOKEN_NAME_CASE(LEFT_BRACE);
        LOX_TOKEN_NAME_CASE(RIGHT_BRACE);
        LOX_TOKEN_NAME_CASE(COMMA);
        LOX_TOKEN_NAME_CASE(DOT);
        LOX_TOKEN_NAME_CASE(MINUS);
        LOX_TOKEN_NAME_CASE(PLUS);
        LOX_TOKEN_NAME_CASE(SEMICOLON);
        LOX_TOKEN_NAME_CASE(SLASH);
        LOX_TOKEN_NAME_CASE(STAR);

        // One or two character tokens.
        LOX_TOKEN_NAME_CASE(BANG);
        LOX_TOKEN_NAME_CASE(BANG_EQUAL);
        LOX_TOKEN_NAME_CASE(EQUAL);
        LOX_TOKEN_NAME_CASE(EQUAL_EQUAL);
        LOX_TOKEN_NAME_CASE(GREATER);
        LOX_TOKEN_NAME_CASE(GREATER_EQUAL);
        LOX_TOKEN_NAME_CASE(LESS);
        LOX_TOKEN_NAME_CASE(LESS_EQUAL);

        // Literals.
        LOX_TOKEN_NAME_CASE(IDENTIFIER);
        LOX_TOKEN_NAME_CASE(STRING);
        LOX_TOKEN_NAME_CASE(NUMBER);

        // Keywords.
        LOX_TOKEN_NAME_CASE(AND);
        LOX_TOKEN_NAME_CASE(BREAK);
        LOX_TOKEN_NAME_CASE(CLASS);
        LOX_TOKEN_NAME_CASE(CONTINUE);
        LOX_TOKEN_NAME_CASE(ELSE);
        LOX_TOKEN_NAME_CASE(FALSE);
        LOX_TOKEN_NAME_CASE(FUN);
        LOX_TOKEN_NAME_CASE(FOR);
        LOX_TOKEN_NAME_CASE(IF);
        LOX_TOKEN_NAME_CASE(NIL);
        LOX_TOKEN_NAME_CASE(OR);
        LOX_TOKEN_NAME_CASE(RETURN);
        LOX_TOKEN_NAME_CASE(SUPER);
        LOX_TOKEN_NAME_CASE(THIS);
        LOX_TOKEN_NAME_CASE(TRUE);
        LOX_TOKEN_NAME_CASE(VAR);
        LOX_TOKEN_NAME_CASE(WHILE);

        LOX_TOKEN_NAME_CASE(END_OF_FILE);
    }

    return "";
}
} // namespace
#endif

Token::Token(Type type, std::string lexeme, size_t line) : type{type}, lexeme{std::move(lexeme)}, line{line}
{}

#ifndef NDEBUG
std::string Token::toString() const
{
    std::ostringstream oss;
    oss << line << " " << getTokenName(type) << "[" << lexeme << "]";
    return oss.str();
}
#endif

} // namespace cloxx
