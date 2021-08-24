#include "Lox.hpp"

#include "Assert.hpp"
#include "GC.hpp"
#include "Interpreter.hpp"
#include "LoxFunction.hpp"
#include "LoxNativeFunction.hpp"
#include "Parser.hpp"
#include "Resolver.hpp"
#include "RuntimeError.hpp"
#include "Scanner.hpp"

#include <iostream>

namespace cloxx {

namespace {
void defineBuiltins(std::shared_ptr<Environment> const& env)
{
    LOX_ASSERT(env);
    env->define("clock", std::make_shared<LoxNativeFunction>(0, [](auto& /*args*/) {
                    auto duration = std::chrono::steady_clock::now().time_since_epoch();
                    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                    return toLoxNumber(millis / 1000.0);
                }));
}
} // namespace

Lox::Lox()
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

    GarbageCollector gc;

    // Define built-in global object such as "clock"
    defineBuiltins(gc.root());

    Interpreter interpreter{this, &gc};

    Resolver resolver{this, &interpreter};
    resolver.resolve(stmts);

    // Stop if there was a resolution error.
    if (_hadError) {
        return 65;
    }

#if 0
    interpreter.interpret(stmts);
#else
    for (auto const& stmt : stmts) {
        interpreter.interpret({stmt});
        gc.collect();
    }
#endif

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
