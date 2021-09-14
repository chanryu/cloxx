#include "LoxClass.hpp"

#include "LoxFunction.hpp"
#include "LoxInstance.hpp"

namespace cloxx {

namespace {
LoxClassId getUniqueClassId()
{
    size_t classIndex = 0;
    return static_cast<LoxClassId>(classIndex++);
}
} // namespace

LoxClass::LoxClass(PrivateCreationTag tag, GarbageCollector* gc, std::string name,
                   std::shared_ptr<LoxClass> const& superclass,
                   std::map<std::string, std::shared_ptr<LoxFunction>> methods)
    : Traceable{tag}, _classId{getUniqueClassId()}, _gc{gc}, _name{std::move(name)},
      _superclass{superclass}, _methods{std::move(methods)}
{
    for (auto& [_, method] : _methods) {
        method->setClassId(_classId);
    }
}

LoxClassId LoxClass::classId() const
{
    return _classId;
}

std::shared_ptr<LoxClass> LoxClass::superclass() const
{
    return _superclass;
}

std::shared_ptr<LoxFunction> LoxClass::findMethod(std::string const& name) const
{
    if (auto it = _methods.find(name); it != _methods.end()) {
        return it->second;
    }

    if (_superclass) {
        return _superclass->findMethod(name);
    }

    return nullptr;
}

std::string LoxClass::toString()
{
    return _name;
}

size_t LoxClass::arity() const
{
    if (auto initializer = findMethod("init")) {
        return initializer->arity();
    }

    return 0;
}

std::shared_ptr<LoxObject> LoxClass::call(std::vector<std::shared_ptr<LoxObject>> const& args)
{
    auto instance = _gc->create<LoxInstance>(shared_from_this());

    if (auto initializer = findMethod("init")) {
        initializer->bind(instance)->call(args);
    }

    return instance;
}

void LoxClass::enumerateTraceables(Traceable::Enumerator const& enumerator)
{
    if (_superclass) {
        enumerator.enumerate(*_superclass);
    }

    for (auto& [_, method] : _methods) {
        enumerator.enumerate(*method);
    }
}

void LoxClass::reclaim()
{
    _superclass.reset();
    _methods.clear();
}

} // namespace cloxx
