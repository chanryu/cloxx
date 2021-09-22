#pragma once

#include <memory>
#include <optional>
#include <string>

namespace cloxx {

struct Token {

    enum Type {
        // Single-character tokens.
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,

        // One or two character tokens.
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Literals.
        IDENTIFIER,
        STRING,
        NUMBER,

        // Keywords.
        AND,
        AS,
        BREAK,
        CLASS,
        CONTINUE,
        ELSE,
        FALSE,
        FUN,
        FOR,
        FROM,
        IF,
        IMPORT,
        NIL,
        OR,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,

        END_OF_FILE
    };

    Token(Type type, std::string lexeme, size_t line);

    bool operator<(Token const& rhs) const
    {
        if (type != rhs.type) {
            return type < rhs.type;
        }
        if (lexeme != rhs.lexeme) {
            return lexeme < rhs.lexeme;
        }
        return line < rhs.line;
    }

#ifndef NDEBUG
    std::string toString() const;
#endif

    Type type;
    std::string lexeme;
    size_t line;
};

std::optional<Token::Type> lookupKeyword(std::string const& identifier);

} // namespace cloxx