#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "ast/Expr.hpp"
#include "ast/Stmt.hpp"

#include "Scanner.hpp"

namespace cloxx {

class Lox;

class Parser {
public:
    explicit Parser(Lox* lox);

    std::vector<std::shared_ptr<Stmt>> parse();

private:
    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> varDeclaration();
    std::shared_ptr<FunStmt> function(std::string const& kind);
    std::shared_ptr<Stmt> classDeclaration();
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> whileStatement();
    std::shared_ptr<Stmt> forStatement();
    std::shared_ptr<Stmt> returnStatement();
    std::shared_ptr<Stmt> printStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::vector<std::shared_ptr<Stmt>> block();
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> logicalOr();
    std::shared_ptr<Expr> logicalAnd();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr> const& callee);
    std::shared_ptr<Expr> primary();

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

    void readTokens();

    struct ParseError : std::runtime_error {
        ParseError() : std::runtime_error{"ParseError"} {};
    };
    ParseError error(Token const& token, std::string_view message);
    void synchronize();

    Lox* const _lox;
    Scanner _scanner;
    std::vector<Token> _tokens;
    size_t _current = 0;
};

} // namespace cloxx
