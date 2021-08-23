#pragma once

#include <iostream>
#include <string>
#include <string_view>

#include "GC.hpp"

namespace cloxx {

struct Token;

class Environment;
class Traceable;
class RuntimeError;

class Lox {
public:
    Lox();

    template <typename T, typename... Args>
    std::shared_ptr<T> create(Args&&... args)
    {
        auto traceable = std::make_shared<T>(std::forward<Args>(args)...);
        _gc.addTraceable(traceable);
        return traceable;
    }

public:
    int run(std::string source);

    void error(size_t line, std::string_view message);
    void error(Token const& token, std::string_view message);
    void runtimeError(RuntimeError const& error);

private:
    void report(size_t line, std::string_view where, std::string_view message);

    void defineBuiltins();

    bool _hadError = false;
    bool _hadRuntimeError = false;

    std::shared_ptr<Environment> const _globals;

    // GC support
    GarbageCollector _gc;
};

} // namespace cloxx
