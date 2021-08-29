#include "Parser.hpp"

#include "Assert.hpp"
#include "Lox.hpp"

#define LOX_ASSERT_PREVIOUS(__t) LOX_ASSERT(previous().type == Token::__t)

namespace cloxx {

Parser::Parser(Lox* lox, SourceReader* sourceReader) : _lox{lox}, _scanner{lox, sourceReader}
{
    _current = _scanner.scanToken();
}

std::shared_ptr<Stmt> Parser::parse()
{
    if (isAtEnd()) {
        return nullptr;
    }

    return declaration();
}

std::shared_ptr<Stmt> Parser::declaration()
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
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::varDeclaration()
{
    // varDecl → "var" IDENTIFIER ( "=" expression )? ";" ;

    auto name = consume(Token::IDENTIFIER, "Expect variable name.");

    std::shared_ptr<Expr> initializer;
    if (match(Token::EQUAL)) {
        initializer = expression();
    }

    consume(Token::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<VarStmt>(name, initializer);
}

std::shared_ptr<FunStmt> Parser::function(std::string const& kind)
{
    // function   → IDENTIFIER "(" parameters? ")" block ;
    // parameters → IDENTIFIER ( "," IDENTIFIER )* ;

    auto name = consume(Token::IDENTIFIER, "Expect " + kind + " name.");
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

    return std::make_shared<FunStmt>(name, params, body);
}

std::shared_ptr<Stmt> Parser::classDeclaration()
{
    LOX_ASSERT_PREVIOUS(CLASS);

    // classDecl → "class" IDENTIFIER ( "<" IDENTIFIER )?
    //             "{" function* "}" ;

    auto name = consume(Token::IDENTIFIER, "Expect class name.");

    std::shared_ptr<VariableExpr> superclass;
    if (match(Token::LESS)) {
        consume(Token::IDENTIFIER, "Expect superclass name.");
        superclass = std::make_shared<VariableExpr>(previous());
    }

    consume(Token::LEFT_BRACE, "Expect '{' before class body.");

    std::vector<std::shared_ptr<FunStmt>> methods;
    while (!check(Token::RIGHT_BRACE) && !isAtEnd()) {
        methods.push_back(function("method"));
    }

    consume(Token::RIGHT_BRACE, "Expect '}' after class body.");

    return std::make_shared<ClassStmt>(name, superclass, methods);
}

std::shared_ptr<Stmt> Parser::statement()
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
        return std::make_shared<BlockStmt>(block());
    }
    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::ifStatement()
{
    LOX_ASSERT_PREVIOUS(IF);

    // ifStmt → "if" "(" expression ")" statement ( "else" statement )? ;

    consume(Token::LEFT_PAREN, "Expect '(' after 'if'.");
    auto const condition = expression();
    consume(Token::RIGHT_PAREN, "Expect ')' after if condition.");

    std::shared_ptr<Stmt> thenBranch = statement();
    std::shared_ptr<Stmt> elseBranch;
    if (match(Token::ELSE)) {
        elseBranch = statement();
    }

    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Stmt> Parser::whileStatement()
{
    LOX_ASSERT_PREVIOUS(WHILE);

    // whileStmt → "while" "(" expression ")" statement ;

    consume(Token::LEFT_PAREN, "Expect '(' after 'while'.");
    auto const cond = expression();
    consume(Token::RIGHT_PAREN, "Expect ')' after condition.");

    auto const body = statement();

    return std::make_shared<WhileStmt>(cond, body);
}

std::shared_ptr<Stmt> Parser::forStatement()
{
    LOX_ASSERT_PREVIOUS(FOR);

    // forStmt → "for" "(" ( varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement ;

    consume(Token::LEFT_PAREN, "Expect '(' after 'for'.");

    std::shared_ptr<Stmt> initializer;
    if (match(Token::VAR)) {
        initializer = varDeclaration();
    }
    else if (!match(Token::SEMICOLON)) {
        initializer = expressionStatement();
    }

    std::shared_ptr<Expr> condition;
    if (!check(Token::SEMICOLON)) {
        condition = expression();
    }
    consume(Token::SEMICOLON, "Expect ';' after loop condition.");

    std::shared_ptr<Stmt> increment;
    if (!check(Token::RIGHT_PAREN)) {
        increment = std::make_shared<ExprStmt>(expression());
    }
    consume(Token::RIGHT_PAREN, "Expect ')' after for clauses.");

    auto body = statement();

    // For loop desugaring.

    // 1. Append increament to body if needed
    if (increment) {
        body = std::make_shared<BlockStmt>(std::vector<std::shared_ptr<Stmt>>{body, increment});
    }

    // 2. Make condition if missing and buid while statement
    if (!condition) {
        condition = std::make_shared<LiteralExpr>(toLoxBoolean(true));
    }
    body = std::make_shared<WhileStmt>(condition, body);

    // 3. Prepand initializer if needed
    if (initializer) {
        body = std::make_shared<BlockStmt>(std::vector<std::shared_ptr<Stmt>>{initializer, body});
    }

    return body;
}

std::shared_ptr<Stmt> Parser::returnStatement()
{
    LOX_ASSERT_PREVIOUS(RETURN);

    auto keyword = previous();

    std::shared_ptr<Expr> expr;
    if (!match(Token::SEMICOLON)) {
        expr = expression();
        consume(Token::SEMICOLON, "Expect ';' after return value.");
    }

    return std::make_shared<ReturnStmt>(keyword, expr);
}

std::shared_ptr<Stmt> Parser::printStatement()
{
    auto const value = expression();
    consume(Token::SEMICOLON, "Expect ';' after value.");
    return std::make_shared<PrintStmt>(value);
}

std::shared_ptr<Stmt> Parser::expressionStatement()
{
    auto const expr = expression();
    consume(Token::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExprStmt>(expr);
}

std::vector<std::shared_ptr<Stmt>> Parser::block()
{
    LOX_ASSERT_PREVIOUS(LEFT_BRACE);

    // block → "{" declaration* "}" ;

    std::vector<std::shared_ptr<Stmt>> stmts;
    while (!check(Token::RIGHT_BRACE) && !isAtEnd()) {
        stmts.push_back(declaration());
    }
    consume(Token::RIGHT_BRACE, "Expect '}' after block.");
    return stmts;
}

std::shared_ptr<Expr> Parser::expression()
{
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment()
{
    // assignment → ( call "." )? IDENTIFIER "=" assignment
    //            | logic_or ;

    auto const expr = logicalOr();

    if (match(Token::EQUAL)) {
        auto equals = previous();
        auto value = assignment();

        if (auto const var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_shared<AssignExpr>(var->name, value);
        }

        if (auto const get = dynamic_cast<GetExpr*>(expr.get())) {
            return std::make_shared<SetExpr>(get->object, get->name, value);
        }

        _lox->syntaxError(equals, "Invalid assignment target.");
    }

    return expr;
}

std::shared_ptr<Expr> Parser::logicalOr()
{
    // logic_or → logic_and ( "or" logic_and )* ;

    auto expr = logicalAnd();

    while (match(Token::OR)) {
        auto op = previous();
        auto right = logicalAnd();
        expr = std::make_shared<LogicalExpr>(op, expr, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::logicalAnd()
{
    // logic_and → equality ( "and" equality )* ;

    auto expr = equality();

    while (match(Token::AND)) {
        auto op = previous();
        auto right = equality();
        expr = std::make_shared<LogicalExpr>(op, expr, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::equality()
{
    // equality → comparison ( ( "!=" | "==" ) comparison )* ;

    auto expr = comparison();

    while (match(Token::BANG_EQUAL, Token::EQUAL_EQUAL)) {
        auto op = previous();
        auto right = comparison();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::comparison()
{
    // comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;

    auto expr = term();

    while (match(Token::GREATER, Token::GREATER_EQUAL, Token::LESS, Token::LESS_EQUAL)) {
        auto op = previous();
        auto right = term();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::term()
{
    // term → factor ( ( "-" | "+" ) factor )* ;

    auto expr = factor();

    while (match(Token::MINUS, Token::PLUS)) {
        auto op = previous();
        auto right = factor();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::factor()
{
    auto expr = unary();
    while (match(Token::SLASH, Token::STAR)) {
        auto op = previous();
        auto right = unary();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::unary()
{
    // unary → ( "!" | "-" ) unary | call ;

    if (match(Token::BANG, Token::MINUS)) {
        auto op = previous();
        auto right = unary();
        return std::make_shared<UnaryExpr>(op, right);
    }
    return call();
}

std::shared_ptr<Expr> Parser::call()
{
    // call      → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
    // arguments → expression ( "," expression )* ;

    auto expr = primary();

    while (true) {
        if (match(Token::LEFT_PAREN)) {
            expr = finishCall(expr);
        }
        else if (match(Token::DOT)) {
            auto name = consume(Token::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_shared<GetExpr>(expr, name);
        }
        else {
            break;
        }
    }

    return expr;
}

std::shared_ptr<Expr> Parser::finishCall(std::shared_ptr<Expr> const& callee)
{
    std::vector<std::shared_ptr<Expr>> args;
    if (!check(Token::RIGHT_PAREN)) {
        do {
            if (args.size() >= 255) {
                _lox->syntaxError(peek(), "Can't have more than 255 arguments.");
            }
            args.push_back(expression());
        } while (match(Token::COMMA));
    }

    auto paren = consume(Token::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_shared<CallExpr>(callee, paren, args);
}

std::shared_ptr<Expr> Parser::primary()
{
    // primary → "true" | "false" | "nil"
    //         | NUMBER | STRING
    //         | IDENTIFIER | "(" expression ")"
    //         | "this"
    //         | "super" "." IDENTIFIER ;

    if (match(Token::FALSE)) {
        return std::make_shared<LiteralExpr>(toLoxBoolean(false));
    }

    if (match(Token::TRUE)) {
        return std::make_shared<LiteralExpr>(toLoxBoolean(true));
    }

    if (match(Token::NIL)) {
        return std::make_shared<LiteralExpr>(makeLoxNil());
    }

    if (match(Token::NUMBER)) {
        auto value = std::stod(previous().lexeme);
        auto literal = std::make_shared<LoxNumber>(value);
        return std::make_shared<LiteralExpr>(literal);
    }

    if (match(Token::STRING)) {
        // Trim the surrounding quotes.
        auto const& lexmem = previous().lexeme;
        LOX_ASSERT(lexmem.length() >= 2);
        auto value = lexmem.substr(1, lexmem.size() - 2);
        auto literal = std::make_shared<LoxString>(std::move(value));
        return std::make_shared<LiteralExpr>(literal);
    }

    if (match(Token::LEFT_PAREN)) {
        auto expr = expression();
        consume(Token::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<GroupingExpr>(expr);
    }

    if (match(Token::IDENTIFIER)) {
        return std::make_shared<VariableExpr>(previous());
    }

    if (match(Token::THIS)) {
        return std::make_shared<ThisExpr>(previous());
    }

    if (match(Token::SUPER)) {
        auto keyword = previous();
        consume(Token::DOT, "Expect '.' after 'super'.");
        auto method = consume(Token::IDENTIFIER, "Expect superclass method name.");
        return std::make_shared<SuperExpr>(keyword, method);
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

bool Parser::check(Token::Type type)
{
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool Parser::isAtEnd()
{
    return peek().type == Token::END_OF_FILE;
}

Token const& Parser::advance()
{
    if (!isAtEnd()) {
        _previous = _current;
        _current = _scanner.scanToken();
    }
    return previous();
}

Token const& Parser::peek()
{
    return _current;
}

Token const& Parser::previous() const
{
    return _previous;
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
    _lox->syntaxError(token, message);
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
