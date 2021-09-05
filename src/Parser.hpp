#pragma once

#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include "ast/Expr.hpp"
#include "ast/Stmt.hpp"

#include "Scanner.hpp"

namespace cloxx {

class ErrorReporter;
class SourceReader;

class Parser {
public:
    explicit Parser(ErrorReporter* errorReporter, SourceReader* sourceReader);

    std::optional<Stmt> parse();

private:
    std::optional<Stmt> declaration();
    Stmt varDeclaration();
    FunStmt function(std::string const& kind);
    Stmt classDeclaration();
    Stmt statement();
    Stmt ifStatement();
    Stmt whileStatement();
    Stmt forStatement();
    Stmt returnStatement();
    Stmt expressionStatement();
    std::vector<Stmt> block();
    Expr expression();
    Expr assignment();
    Expr logicalOr();
    Expr logicalAnd();
    Expr equality();
    Expr comparison();
    Expr term();
    Expr factor();
    Expr unary();
    Expr call();
    Expr finishCall(Expr const& callee);
    Expr primary();

    bool match(Token::Type type);
    template <typename... Types>
    bool match(Token::Type type, Types... types)
    {
        if (match(type)) {
            return true;
        }
        return match(types...);
    }

    bool check(Token::Type type);
    bool isAtEnd();
    Token const& advance();
    Token const& peek();
    Token const& previous() const;
    Token const& consume(Token::Type type, std::string_view message);

    struct ParseError : std::runtime_error {
        ParseError() : std::runtime_error{"ParseError"} {};
    };
    ParseError error(Token const& token, std::string_view message);
    void synchronize();

    ErrorReporter* const _errorReporter;
    Scanner _scanner;
    std::optional<Token> _previous;
    std::optional<Token> _current;
};

} // namespace cloxx
