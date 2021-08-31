#pragma once

#include <string_view>

namespace cloxx {

struct Token;

class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;

    virtual void syntaxError(size_t line, std::string_view message) = 0;
    virtual void syntaxError(Token const& token, std::string_view message) = 0;
    virtual void resolveError(Token const& token, std::string_view message) = 0;
    virtual void runtimeError(Token const& token, std::string_view message) = 0;
};

} // namespace cloxx
