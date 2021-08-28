#pragma once

#include <istream>
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

    bool readLine(std::string& line);

    void error(size_t line, std::string_view message);
    void error(Token const& token, std::string_view message);
    void runtimeError(RuntimeError const& error);

private:
    void report(size_t line, std::string_view where, std::string_view message);

    bool _hadError = false;
    bool _hadRuntimeError = false;

    std::istream* _input;
};

} // namespace cloxx
