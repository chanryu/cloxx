#include "Lox.hpp"

#include "Assert.hpp"
#include "ErrorReporter.hpp"
#include "Interpreter.hpp"
#include "LoxBool.hpp"
#include "LoxClass.hpp"
#include "LoxFunction.hpp"
#include "LoxList.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"
#include "LoxNumber.hpp"
#include "Parser.hpp"
#include "Resolver.hpp"
#include "SourceReader.hpp"

#include <chrono> // for clock() native function
#include <fstream>
#include <iostream>
#include <sstream>

namespace cloxx {

namespace {
auto makeBuiltIns(GarbageCollector* gc)
{
    std::map<std::string, std::shared_ptr<LoxObject>> builtIns;

    // global functions
    builtIns.emplace("print", gc->create<LoxNativeFunction>(gc, 1, [](auto& /*instance*/, auto& args) {
        std::cout << args[0]->toString() << '\n';
        return makeLoxNil();
    }));
    builtIns.emplace("clock", gc->create<LoxNativeFunction>(gc, 0, [](auto& /*instance*/, auto& /*args*/) {
        auto duration = std::chrono::steady_clock::now().time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return toLoxNumber(millis / 1000.0);
    }));

    // buil-in classes
    builtIns.emplace("Bool", createBoolClass(gc));
    builtIns.emplace("List", createListClass(gc));

    return builtIns;
}

class IStreamSourceReader : public SourceReader {
public:
    IStreamSourceReader(std::istream& istream) : _istream{istream}
    {
        readNextChar();
    }

    bool isEndOfSource()
    {
        return _nextChar == '\0';
    }

    char readChar()
    {
        char c = _nextChar;
        readNextChar();
        return c;
    }

private:
    void readNextChar()
    {
        if (!_istream.get(_nextChar)) {
            _nextChar = '\0';
        }
    }
    std::istream& _istream;
    char _nextChar;
};

class ConsoleErrorReporter : public ErrorReporter {
public:
    void syntaxError(size_t line, std::string_view message) override
    {
        _syntaxErrorCount += 1;

        std::cerr << "[line " << line << "] Error: " << message << '\n';
    }

    void syntaxError(Token const& token, std::string_view message) override
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

    void resolveError(Token const& token, std::string_view message) override
    {
        _resolveErrorCount += 1;

        std::cerr << "[line " << token.line << "] ";
        std::cerr << "Error at '" << token.lexeme << "': ";
        std::cerr << message << '\n';
    }

    void runtimeError(Token const& token, std::string_view message) override
    {
        _hadRuntimeError = true;

        std::cerr << "[line " << token.line << "] ";
        std::cerr << message << '\n';
    }

    unsigned int syntaxErrorCount() const
    {
        return _syntaxErrorCount;
    }

    unsigned int resolveErrorCount() const
    {
        return _resolveErrorCount;
    }

    bool hadRuntimeError() const
    {
        return _hadRuntimeError;
    }

private:
    unsigned int _syntaxErrorCount = 0;
    unsigned int _resolveErrorCount = 0;
    bool _hadRuntimeError = false;
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
    ConsoleErrorReporter errorReporter;

    Parser parser{&errorReporter, &sourceReader};

    Resolver resolver{&errorReporter};
    Interpreter interpreter{&errorReporter, makeBuiltIns};

    while (true) {
        auto const prevSyntaxErrorCount = errorReporter.syntaxErrorCount();
        auto const stmt = parser.parse();
        if (!stmt) {
            if (errorReporter.syntaxErrorCount() > prevSyntaxErrorCount) {
                // Scanning or parsing error(s) - continue on to report more errors
                continue;
            }

            // We're done!
            break;
        }

        if (errorReporter.syntaxErrorCount() != 0) {
            continue;
        }

        if (!resolver.resolve(*stmt)) {
            // Resolution or analytical error(s) - continue on to report more errors
            continue;
        }

        if (errorReporter.resolveErrorCount() != 0 || errorReporter.hadRuntimeError()) {
            continue;
        }

        interpreter.interpret(*stmt);
    }

    // Indicate a syntax / resolve error in the exit code.
    if (errorReporter.syntaxErrorCount() > 0 || errorReporter.resolveErrorCount() > 0) {
        return 65;
    }

    // Indicate a run-time error in the exit code.
    if (errorReporter.hadRuntimeError()) {
        return 70;
    }

    return 0;
}

} // namespace cloxx
