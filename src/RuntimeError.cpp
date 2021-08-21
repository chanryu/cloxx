#include "RuntimeError.hpp"

namespace cloxx {

RuntimeError::RuntimeError(Token const& token, char const* message) : std::runtime_error{message}, token{token}
{}

RuntimeError::RuntimeError(Token const& token, std::string const& message) : std::runtime_error{message}, token{token}
{}

} // namespace cloxx
