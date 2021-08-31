#pragma once

#include <string>

#include "ErrorReporter.hpp"

namespace cloxx {

struct Token;

class Environment;
class Traceable;
class SourceReader;

class Lox : public ErrorReporter {
public:
    Lox();

public:
    int run(SourceReader& sourceReader);
    int runFile(char const* filepath);

    void syntaxError(size_t line, std::string_view message) override;
    void syntaxError(Token const& token, std::string_view message) override;
    void resolveError(Token const& token, std::string_view message) override;
    void runtimeError(Token const& token, std::string_view message) override;

private:
    unsigned int _syntaxErrorCount = 0;
    unsigned int _resolveErrorCount = 0;

    bool _hadRuntimeError = false;
};

} // namespace cloxx
