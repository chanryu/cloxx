#pragma once

#include <stdexcept>

#include "Token.hpp"

namespace cloxx {

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(Token const& token, char const* message);
    RuntimeError(Token const& token, std::string const& message);

    Token const token;
};

} // namespace cloxx
