#include "LoxInstance.hpp"

#include "Assert.hpp"
#include "LoxClass.hpp"
#include "LoxFunction.hpp"
#include "RuntimeError.hpp"
#include "Token.hpp"

namespace cloxx {

LoxInstance::LoxInstance(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass) : Traceable{tag}, _class{klass}
{
    LOX_ASSERT(_class);

    for (auto klass = _class; klass; klass = klass->superclass()) {
        auto nativeData = klass->createNativeData();
        if (nativeData) {
            _nativeDataMap.emplace(klass->classId(), nativeData);
        }
    }
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

std::shared_ptr<Traceable> LoxInstance::getNativeData(LoxClassId classId) const
{
    if (auto it = _nativeDataMap.find(classId); it != _nativeDataMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::string LoxInstance::toString()
{
    if (auto method = _class->findMethod("toString")) {
        return method->bind(shared_from_this())->call({})->toString();
    }

    return _class->toString() + " instance";
}

void LoxInstance::enumerateTraceables(Enumerator const& enumerator)
{
    enumerator.enumerate(*_class);

    for (auto& [_, field] : _fields) {
        if (auto traceable = dynamic_cast<Traceable*>(field.get())) {
            enumerator.enumerate(*traceable);
        }
    }

    for (auto& [_, nativeData] : _nativeDataMap) {
        enumerator.enumerate(*nativeData);
    }
}

void LoxInstance::reclaim()
{
    _class.reset();
    _fields.clear();
    _nativeDataMap.clear();
}

} // namespace cloxx
