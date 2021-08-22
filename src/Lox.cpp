#include "Lox.hpp"

#include "Interpreter.hpp"
#include "LoxFunction.hpp"
#include "Parser.hpp"
#include "Resolver.hpp"
#include "RuntimeError.hpp"
#include "Scanner.hpp"
#include "Traceable.hpp"

#include <iostream>

namespace cloxx {

Lox::Lox() : _globals{std::make_shared<Environment>()}, _gc{_globals}
{}

int Lox::run(std::string source)
{
    Scanner scanner{this, std::move(source)};
    Parser parser{this, scanner.scanTokens()};

    auto stmts = parser.parse();

    // Indicate a syntax error in the exit code.
    if (_hadError) {
        return 65;
    }

    Interpreter interpreter{this, _globals};

    Resolver resolver{this, &interpreter};
    resolver.resolve(stmts);

    // Stop if there was a resolution error.
    if (_hadError) {
        return 65;
    }

    interpreter.interpret(stmts);

    // Indicate a run-time error in the exit code.
    if (_hadRuntimeError) {
        return 70;
    }

    return 0;
}

void Lox::error(size_t line, std::string_view message)
{
    report(line, "", message);
}

void Lox::error(Token const& token, std::string_view message)
{
    if (token.type == Token::END_OF_FILE) {
        report(token.line, "at end", message);
    }
    else {
        report(token.line, "at '" + token.lexeme + "'", message);
    }
}

void Lox::runtimeError(RuntimeError const& error)
{
    std::cerr << "[line " << error.token.line << "] ";
    std::cerr << error.what() << '\n';

    _hadRuntimeError = true;
}

void Lox::report(size_t line, std::string_view where, std::string_view message)
{
    std::cerr << "[line " << line << "] ";
    if (where.empty()) {
        std::cerr << "Error: ";
    }
    else {
        std::cerr << "Error " << where << ": ";
    }
    std::cerr << message << '\n';

    _hadError = true;
}

} // namespace cloxx
