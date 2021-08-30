#include "Lox.hpp"

#include "Assert.hpp"
#include "GC.hpp"
#include "Interpreter.hpp"
#include "LoxFunction.hpp"
#include "LoxNativeFunction.hpp"
#include "Parser.hpp"
#include "Resolver.hpp"
#include "RuntimeError.hpp"
#include "SourceReader.hpp"

#include <chrono> // for clock() native function
#include <fstream>
#include <iostream>
#include <sstream>

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

class IStreamSourceReader : public SourceReader {
public:
    IStreamSourceReader(std::istream& istream) : _istream{istream}
    {}

    bool readSource(std::string& line)
    {
        if (!_istream.eof()) {
            std::getline(_istream, line);
            if (!_istream.eof()) {
                line.push_back('\n');
            }
        }

        return !_istream.eof();
    }

private:
    std::istream& _istream;
};

} // namespace

Lox::Lox()
{}

int Lox::runFile(char const* filepath)
{
    std::ifstream ifs{filepath};
    if (!ifs.is_open()) {
        std::cerr << "Error: Cannot open file '" << filepath << "' to read!\n";
        return 1;
    }

    IStreamSourceReader sourceReader{ifs};

    return run(sourceReader);
}

int Lox::run(SourceReader& sourceReader)
{
    _syntaxErrorCount = 0;
    _resolveErrorCount = 0;
    _hadRuntimeError = false;

    Parser parser{this, &sourceReader};

    GarbageCollector gc;

    // Define built-in global object such as "clock"
    defineBuiltins(gc.root());

    Interpreter interpreter{this, &gc};
    Resolver resolver{this, &interpreter};

    while (true) {
        auto const prevSyntaxErrorCount = _syntaxErrorCount;
        auto const stmt = parser.parse();
        if (!stmt) {
            if (_syntaxErrorCount > prevSyntaxErrorCount) {
                // Scanning or parsing error(s) - continue on to report more errors
                continue;
            }

            // We're done!
            break;
        }

        if (_syntaxErrorCount != 0) {
            continue;
        }

        if (!resolver.resolve(*stmt)) {
            // Resolution or analytical error(s) - continue on to report more errors
            continue;
        }

        if (_resolveErrorCount != 0 || _hadRuntimeError) {
            continue;
        }

        interpreter.interpret(*stmt);
        gc.collect();
    }

    // Indicate a syntax / resolve error in the exit code.
    if (_syntaxErrorCount > 0 || _resolveErrorCount > 0) {
        return 65;
    }

    // Indicate a run-time error in the exit code.
    if (_hadRuntimeError) {
        return 70;
    }

    return 0;
}

void Lox::syntaxError(size_t line, std::string_view message)
{
    _syntaxErrorCount += 1;

    std::cerr << "[line " << line << "] Error: " << message << '\n';
}

void Lox::syntaxError(Token const& token, std::string_view message)
{
    _syntaxErrorCount += 1;

    std::cerr << "[line " << token.line << "] ";
    if (token.type == Token::END_OF_FILE) {
        std::cerr << "Error at end: ";
    }
    else {
        std::cerr << "Error at '" << token.lexeme << "': ";
    }
    std::cerr << message << '\n';
}

void Lox::resolveError(Token const& token, std::string_view message)
{
    _resolveErrorCount += 1;

    std::cerr << "[line " << token.line << "] ";
    std::cerr << "Error at '" << token.lexeme << "': ";
    std::cerr << message << '\n';
}

void Lox::runtimeError(RuntimeError const& error)
{
    _hadRuntimeError = true;

    std::cerr << "[line " << error.token.line << "] ";
    std::cerr << error.what() << '\n';
}

} // namespace cloxx
