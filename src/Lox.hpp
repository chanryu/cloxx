#pragma once

#include <iostream>
#include <string>
#include <string_view>

#include <memory> // GC support
#include <vector> // GC support

namespace cloxx {

struct Token;

class Traceable;
class RuntimeError;

class Lox {
public:
    // GC support
    template <typename T, typename... Args>
    std::shared_ptr<T> create(Args&&... args)
    {
        auto traceable = std::make_shared<T>(std::forward<Args>(args)...);
        _traceables.push_back(traceable);
        return traceable;
    }

    size_t traceableSize() const;
    void reclaimTraceables();

public:
    int run(std::string source);

    void error(size_t line, std::string_view message);
    void error(Token const& token, std::string_view message);
    void runtimeError(RuntimeError const& error);

private:
    void report(size_t line, std::string_view where, std::string_view message);

    bool _hadError = false;
    bool _hadRuntimeError = false;

    // GC support
    std::vector<std::shared_ptr<Traceable>> _traceables;
};

} // namespace cloxx
