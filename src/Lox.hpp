#pragma once

#include <string>
#include <string_view>

namespace cloxx {

struct Token;

class Environment;
class Traceable;
class RuntimeError;

class Lox {
public:
    Lox();

public:
    int run(std::string source);

    void syntaxError(size_t line, std::string_view message);
    void syntaxError(Token const& token, std::string_view message);
    void resolveError(Token const& token, std::string_view message);
    void runtimeError(RuntimeError const& error);

private:
    unsigned int _syntaxErrorCount = 0;
    unsigned int _resolveErrorCount = 0;

    bool _hadRuntimeError = false;
};

} // namespace cloxx
