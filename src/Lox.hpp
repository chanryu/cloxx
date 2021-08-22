#pragma once

#include <iostream>
#include <string>
#include <string_view>

#include <memory> // GC support
#include <vector> // GC support

namespace cloxx {

struct Token;

class Environment;
class Traceable;
class RuntimeError;

class Lox {
public:
    Lox();

    // GC support
    template <typename T, typename... Args>
    std::shared_ptr<T> create(Args&&... args)
    {
        auto traceable = std::make_shared<T>(std::forward<Args>(args)...);
        _traceables.push_back(traceable);
        return traceable;
    }

    size_t traceableSize() const;
    void collectGarbage();
    void reclaimAllTraceables();

public:
    int run(std::string source);

    void error(size_t line, std::string_view message);
    void error(Token const& token, std::string_view message);
    void runtimeError(RuntimeError const& error);

private:
    void report(size_t line, std::string_view where, std::string_view message);

    bool _hadError = false;
    bool _hadRuntimeError = false;

    std::shared_ptr<Environment> const _globals;

    // GC support
    std::vector<std::weak_ptr<Traceable>> _traceables;
};

} // namespace cloxx
