#include "LoxInstance.hpp"

#include "Assert.hpp"
#include "LoxClass.hpp"
#include "LoxFunction.hpp"
#include "RuntimeError.hpp"
#include "Token.hpp"

namespace cloxx {

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> const& klass) : _class{klass}
{}

std::shared_ptr<LoxObject> LoxInstance::get(Token const& name)
{
    if (auto it = _fields.find(name.lexeme); it != _fields.end()) {
        return it->second;
    }

    if (auto method = _class->findMethod(name.lexeme)) {
        return method->bind(std::dynamic_pointer_cast<LoxInstance>(shared_from_this()));
    }

    throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(Token const& name, std::shared_ptr<LoxObject> const& value)
{
    _fields[name.lexeme] = value;
}

std::string LoxInstance::toString() const
{
    return _class->name + " instance";
}

} // namespace cloxx
