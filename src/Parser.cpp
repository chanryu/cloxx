#include "Parser.hpp"

#include "Assert.hpp"
#include "Lox.hpp"

#define LOX_ASSERT_PREVIOUS(__t) LOX_ASSERT(previous().type == Token::__t)

namespace cloxx {

Parser::Parser(Lox* lox, std::vector<Token> tokens) : _lox{lox}, _tokens{std::move(tokens)}
{}

std::vector<Stmt> Parser::parse()
{
    std::vector<Stmt> stmts;
    while (!isAtEnd()) {
        auto stmt = declaration();
        if (!stmt)
            break;
        stmts.push_back(*stmt);
    }
    return stmts;
}

std::optional<Stmt> Parser::declaration()
{
    // declaration → varDecl
    //             | funcDecl
    //             | classDecl
    //             | statement ;

    try {
        if (match(Token::VAR)) {
            return varDeclaration();
        }
        if (match(Token::FUN)) {
            return function("function");
        }
        if (match(Token::CLASS)) {
            return classDeclaration();
        }
        return statement();
    }
    catch (ParseError& error) {
        synchronize();
        return std::nullopt;
    }
}

Stmt Parser::varDeclaration()
{
    // varDecl → "var" IDENTIFIER ( "=" expression )? ";" ;

    auto const& name = consume(Token::IDENTIFIER, "Expect variable name.");

    std::optional<Expr> initializer;
    if (match(Token::EQUAL)) {
        initializer = expression();
    }

    consume(Token::SEMICOLON, "Expect ';' after variable declaration.");
    return makeVarStmt(name, initializer);
}

FunStmt Parser::function(std::string const& kind)
{
    // function   → IDENTIFIER "(" parameters? ")" block ;
    // parameters → IDENTIFIER ( "," IDENTIFIER )* ;

    auto const& name = consume(Token::IDENTIFIER, "Expect " + kind + " name.");
    consume(Token::LEFT_PAREN, "Expect '(' after " + kind + " name.");

    std::vector<Token> params;
    if (!check(Token::RIGHT_PAREN)) {
        do {
            if (params.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }
            params.push_back(consume(Token::IDENTIFIER, "Expect parameter name."));
        } while (match(Token::COMMA));
    }
    consume(Token::RIGHT_PAREN, "Expect ')' after parameters.");

    consume(Token::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    auto const body = block();

    return makeFunStmt(name, params, body);
}

Stmt Parser::classDeclaration()
{
    LOX_ASSERT_PREVIOUS(CLASS);

    // classDecl → "class" IDENTIFIER ( "<" IDENTIFIER )?
    //             "{" function* "}" ;

    auto const& name = consume(Token::IDENTIFIER, "Expect class name.");

    std::optional<VariableExpr> superclass;
    if (match(Token::LESS)) {
        consume(Token::IDENTIFIER, "Expect superclass name.");
        superclass = makeVariableExpr(previous());
    }

    consume(Token::LEFT_BRACE, "Expect '{' before class body.");

    std::vector<FunStmt> methods;
    while (!check(Token::RIGHT_BRACE) && !isAtEnd()) {
        methods.push_back(function("method"));
    }

    consume(Token::RIGHT_BRACE, "Expect '}' after class body.");

    return makeClassStmt(name, superclass, methods);
}

Stmt Parser::statement()
{
    // statement → ifStmt
    //           | whileStmt
    //           | forStmt
    //           | returnStmt
    //           | printStmt
    //           | exprStmt
    //           | block ;

    if (match(Token::IF)) {
        return ifStatement();
    }
    if (match(Token::WHILE)) {
        return whileStatement();
    }
    if (match(Token::FOR)) {
        return forStatement();
    }
    if (match(Token::RETURN)) {
        return returnStatement();
    }
    if (match(Token::PRINT)) {
        return printStatement();
    }
    if (match(Token::LEFT_BRACE)) {
        return makeBlockStmt(block());
    }
    return expressionStatement();
}

Stmt Parser::ifStatement()
{
    LOX_ASSERT_PREVIOUS(IF);

    // ifStmt → "if" "(" expression ")" statement ( "else" statement )? ;

    consume(Token::LEFT_PAREN, "Expect '(' after 'if'.");
    auto const condition = expression();
    consume(Token::RIGHT_PAREN, "Expect ')' after if condition.");

    Stmt thenBranch = statement();
    std::optional<Stmt> elseBranch;
    if (match(Token::ELSE)) {
        elseBranch = statement();
    }

    return makeIfStmt(condition, thenBranch, elseBranch);
}

Stmt Parser::whileStatement()
{
    LOX_ASSERT_PREVIOUS(WHILE);

    // whileStmt → "while" "(" expression ")" statement ;

    consume(Token::LEFT_PAREN, "Expect '(' after 'while'.");
    auto const cond = expression();
    consume(Token::RIGHT_PAREN, "Expect ')' after condition.");

    auto const body = statement();

    return makeWhileStmt(cond, body);
}

Stmt Parser::forStatement()
{
    LOX_ASSERT_PREVIOUS(FOR);

    // forStmt → "for" "(" ( varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement ;

    consume(Token::LEFT_PAREN, "Expect '(' after 'for'.");

    std::optional<Stmt> initializer;
    if (match(Token::VAR)) {
        initializer = varDeclaration();
    }
    else if (!match(Token::SEMICOLON)) {
        initializer = expressionStatement();
    }

    std::optional<Expr> condition;
    if (!check(Token::SEMICOLON)) {
        condition = expression();
    }
    consume(Token::SEMICOLON, "Expect ';' after loop condition.");

    std::optional<Stmt> increment;
    if (!check(Token::RIGHT_PAREN)) {
        increment = makeExprStmt(expression());
    }
    consume(Token::RIGHT_PAREN, "Expect ')' after for clauses.");

    auto body = statement();

    // For loop desugaring.

    // 1. Append increament to body if needed
    if (increment) {
        body = makeBlockStmt(std::vector<Stmt>{body, *increment});
    }

    // 2. Make condition if missing and buid while statement
    if (!condition) {
        condition = makeLiteralExpr(toLoxBoolean(true));
    }
    body = makeWhileStmt(*condition, body);

    // 3. Prepand initializer if needed
    if (initializer) {
        body = makeBlockStmt(std::vector<Stmt>{*initializer, body});
    }

    return body;
}

Stmt Parser::returnStatement()
{
    LOX_ASSERT_PREVIOUS(RETURN);

    auto const& keyword = previous();

    std::optional<Expr> expr;
    if (!match(Token::SEMICOLON)) {
        expr = expression();
        consume(Token::SEMICOLON, "Expect ';' after return value.");
    }

    return makeReturnStmt(keyword, expr);
}

Stmt Parser::printStatement()
{
    auto const value = expression();
    consume(Token::SEMICOLON, "Expect ';' after value.");
    return makePrintStmt(value);
}

Stmt Parser::expressionStatement()
{
    auto const expr = expression();
    consume(Token::SEMICOLON, "Expect ';' after expression.");
    return makeExprStmt(expr);
}

std::vector<Stmt> Parser::block()
{
    LOX_ASSERT_PREVIOUS(LEFT_BRACE);

    // block → "{" declaration* "}" ;

    std::vector<Stmt> stmts;
    while (!check(Token::RIGHT_BRACE) && !isAtEnd()) {
        auto stmt = declaration();
        if (!stmt) {
            // continue on to report more errors
            continue;
        }
        stmts.push_back(*stmt);
    }
    consume(Token::RIGHT_BRACE, "Expect '}' after block.");
    return stmts;
}

Expr Parser::expression()
{
    return assignment();
}

Expr Parser::assignment()
{
    // assignment → ( call "." )? IDENTIFIER "=" assignment
    //            | logic_or ;

    auto const expr = logicalOr();

    if (match(Token::EQUAL)) {
        auto const& equals = previous();
        auto const& value = assignment();

        if (auto const var = expr.toVariableExpr()) {
            return makeAssignExpr(var->name(), value);
        }

        if (auto const get = expr.toGetExpr()) {
            return makeSetExpr(get->object(), get->name(), value);
        }

        _lox->error(equals, "Invalid assignment target.");
    }

    return expr;
}

Expr Parser::logicalOr()
{
    // logic_or → logic_and ( "or" logic_and )* ;

    auto expr = logicalAnd();

    while (match(Token::OR)) {
        auto const& op = previous();
        auto const& right = logicalAnd();
        expr = makeLogicalExpr(op, expr, right);
    }

    return expr;
}

Expr Parser::logicalAnd()
{
    // logic_and → equality ( "and" equality )* ;

    auto expr = equality();

    while (match(Token::AND)) {
        auto const& op = previous();
        auto const& right = equality();
        expr = makeLogicalExpr(op, expr, right);
    }

    return expr;
}

Expr Parser::equality()
{
    // equality → comparison ( ( "!=" | "==" ) comparison )* ;

    auto expr = comparison();

    while (match(Token::BANG_EQUAL, Token::EQUAL_EQUAL)) {
        auto const& op = previous();
        auto const& right = comparison();
        expr = makeBinaryExpr(op, expr, right);
    }

    return expr;
}

Expr Parser::comparison()
{
    // comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;

    auto expr = term();

    while (match(Token::GREATER, Token::GREATER_EQUAL, Token::LESS, Token::LESS_EQUAL)) {
        auto const& op = previous();
        auto const& right = term();
        expr = makeBinaryExpr(op, expr, right);
    }

    return expr;
}

Expr Parser::term()
{
    // term → factor ( ( "-" | "+" ) factor )* ;

    auto expr = factor();

    while (match(Token::MINUS, Token::PLUS)) {
        auto const& op = previous();
        auto const& right = factor();
        expr = makeBinaryExpr(op, expr, right);
    }

    return expr;
}

Expr Parser::factor()
{
    auto expr = unary();
    while (match(Token::SLASH, Token::STAR)) {
        auto const& op = previous();
        auto const& right = unary();
        expr = makeBinaryExpr(op, expr, right);
    }
    return expr;
}

Expr Parser::unary()
{
    // unary → ( "!" | "-" ) unary | call ;

    if (match(Token::BANG, Token::MINUS)) {
        auto const& op = previous();
        auto const& right = unary();
        return makeUnaryExpr(op, right);
    }
    return call();
}

Expr Parser::call()
{
    // call      → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
    // arguments → expression ( "," expression )* ;

    auto expr = primary();

    while (true) {
        if (match(Token::LEFT_PAREN)) {
            expr = finishCall(expr);
        }
        else if (match(Token::DOT)) {
            auto const& name = consume(Token::IDENTIFIER, "Expect property name after '.'.");
            expr = makeGetExpr(expr, name);
        }
        else {
            break;
        }
    }

    return expr;
}

Expr Parser::finishCall(Expr const& callee)
{
    std::vector<Expr> args;
    if (!check(Token::RIGHT_PAREN)) {
        do {
            if (args.size() >= 255) {
                _lox->error(peek(), "Can't have more than 255 arguments.");
            }
            args.push_back(expression());
        } while (match(Token::COMMA));
    }

    auto const& paren = consume(Token::RIGHT_PAREN, "Expect ')' after arguments.");
    return makeCallExpr(callee, paren, args);
}

Expr Parser::primary()
{
    // primary → "true" | "false" | "nil"
    //         | NUMBER | STRING
    //         | IDENTIFIER | "(" expression ")"
    //         | "this"
    //         | "super" "." IDENTIFIER ;

    if (match(Token::FALSE)) {
        return makeLiteralExpr(toLoxBoolean(false));
    }

    if (match(Token::TRUE)) {
        return makeLiteralExpr(toLoxBoolean(true));
    }

    if (match(Token::NIL)) {
        return makeLiteralExpr(makeLoxNil());
    }

    if (match(Token::NUMBER, Token::STRING)) {
        return makeLiteralExpr(previous().literal);
    }

    if (match(Token::LEFT_PAREN)) {
        auto const& expr = expression();
        consume(Token::RIGHT_PAREN, "Expect ')' after expression.");
        return makeGroupingExpr(expr);
    }

    if (match(Token::IDENTIFIER)) {
        return makeVariableExpr(previous());
    }

    if (match(Token::THIS)) {
        return makeThisExpr(previous());
    }

    if (match(Token::SUPER)) {
        auto const& keyword = previous();
        consume(Token::DOT, "Expect '.' after 'super'.");
        auto const& method = consume(Token::IDENTIFIER, "Expect superclass method name.");
        return makeSuperExpr(keyword, method);
    }

    throw error(peek(), "Expect expression.");
}

bool Parser::match(Token::Type type)
{
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type type) const
{
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool Parser::isAtEnd() const
{
    return peek().type == Token::END_OF_FILE;
}

Token const& Parser::advance()
{
    if (!isAtEnd()) {
        _current++;
    }
    return previous();
}

Token const& Parser::peek() const
{
    return _tokens[_current];
}

Token const& Parser::previous() const
{
    return _tokens[_current - 1];
}

Token const& Parser::consume(Token::Type type, std::string_view message)
{
    if (check(type)) {
        return advance();
    }

    throw error(peek(), message);
}

Parser::ParseError Parser::error(Token const& token, std::string_view message)
{
    _lox->error(token, message);
    return ParseError{};
}

void Parser::synchronize()
{
    advance();

    while (!isAtEnd()) {
        if (previous().type == Token::SEMICOLON)
            return;

        switch (peek().type) {
        case Token::CLASS:
        case Token::FUN:
        case Token::VAR:
        case Token::FOR:
        case Token::IF:
        case Token::WHILE:
        case Token::PRINT:
        case Token::RETURN:
            return;
        default:
            break;
        }

        advance();
    }
}
} // namespace cloxx
