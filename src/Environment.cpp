#include "Environment.hpp"

#include "Assert.hpp"
#include "LoxObject.hpp"
#include "RuntimeError.hpp"
#include "Token.hpp"

namespace cloxx {

Environment::Environment(PrivateCreationTag tag) : Traceable{tag}
{}

Environment::Environment(PrivateCreationTag tag, std::shared_ptr<Environment> const& enclosing)
    : Traceable{tag}, _enclosing{enclosing}
{}

void Environment::define(std::string const& name, std::shared_ptr<LoxObject> const& value)
{
    LOX_ASSERT(value);
    _values[name] = value;
}

std::shared_ptr<LoxObject> Environment::get(Token const& name) const
{
    if (auto it = _values.find(name.lexeme); it != _values.end()) {
        return it->second;
    }

    throw RuntimeError{name, "Undefined variable '" + name.lexeme + "'."};
}

void Environment::assign(Token const& name, std::shared_ptr<LoxObject> const& value)
{
    LOX_ASSERT(value);

    if (auto it = _values.find(name.lexeme); it != _values.end()) {
        it->second = value;
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

std::shared_ptr<LoxObject> Environment::getAt(size_t distance, std::string const& name) const
{
    auto& values = ancestor(distance)->_values;
    if (auto it = values.find(name); it != values.end()) {
        return it->second;
    }

    LOX_ASSERT(false); // If we have reached here, we have a scope resolve bug.
    return nullptr;
}

void Environment::assignAt(size_t distance, std::string const& name, std::shared_ptr<LoxObject> const& value)
{
    auto& values = ancestor(distance)->_values;
    if (auto it = values.find(name); it != values.end()) {
        it->second = value;
        return;
    }

    LOX_ASSERT(false); // If we have reached here, we have a scope resolve bug.
}

Environment const* Environment::ancestor(size_t distance) const
{
    auto environment = this;
    for (size_t i = 0; i < distance; i++) {
        environment = environment->_enclosing.get();
        LOX_ASSERT(environment);
    }
    return environment;
}

Environment* Environment::ancestor(size_t distance)
{
    auto environment = this;
    for (size_t i = 0; i < distance; i++) {
        environment = environment->_enclosing.get();
        LOX_ASSERT(environment);
    }
    return environment;
}

void Environment::enumerateTraceables(Traceable::Enumerator const& enumerator)
{
    if (_enclosing) {
        enumerator.enumerate(*_enclosing);
    }

    for (auto& [_, value] : _values) {
        enumerator.enumerate(*value);
    }
}

void Environment::reclaim()
{
    _enclosing.reset();
    _values.clear();
}

} // namespace cloxx
