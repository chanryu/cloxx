#pragma once

#include <memory>
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
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
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

#ifndef NDEBUG
    std::string toString() const;
#endif

    Type type;
    std::string lexeme;
    size_t line;
};

} // namespace cloxx
