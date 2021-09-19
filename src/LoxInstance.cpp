#include "LoxInstance.hpp"

#include "Assert.hpp"
#include "LoxClass.hpp"
#include "LoxFunction.hpp"
#include "RuntimeError.hpp"
#include "Token.hpp"

namespace cloxx {

LoxInstance::LoxInstance(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass) : LoxObject{tag}, _class{klass}
{
    LOX_ASSERT(_class);
}

std::shared_ptr<LoxClass> const& LoxInstance::klass() const
{
    return _class;
}

std::shared_ptr<LoxObject> LoxInstance::get(Token const& name)
{
    if (auto it = _fields.find(name.lexeme); it != _fields.end()) {
        return it->second;
    }

    if (auto method = _class->findMethod(name.lexeme)) {
        return method->bind(shared_from_this());
    }

    throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(Token const& name, std::shared_ptr<LoxObject> const& value)
{
    _fields[name.lexeme] = value;
}

std::string LoxInstance::toString()
{
    if (auto method = _class->findMethod("toString"); method && method->arity() == 0) {
        return method->bind(shared_from_this())->call({})->toString();
    }

    return _class->toString() + " instance";
}

bool LoxInstance::equals(std::shared_ptr<LoxObject> const& object)
{
    if (auto method = _class->findMethod("equals"); method && method->arity() == 1) {
        return method->bind(shared_from_this())->call({object})->isTruthy();
    }

    return this == object.get();
}

void LoxInstance::enumerateTraceables(Enumerator const& enumerator)
{
    enumerator.enumerate(*_class);

    for (auto& [_, field] : _fields) {
        enumerator.enumerate(*field);
    }
}

void LoxInstance::reclaim()
{
    _class.reset();
    _fields.clear();
}

} // namespace cloxx
