#include "LoxClass.hpp"

#include "LoxFunction.hpp"
#include "LoxInstance.hpp"

namespace cloxx {

LoxClass::LoxClass(std::string name, std::shared_ptr<LoxClass> const& superclass,
                   std::map<std::string, std::shared_ptr<LoxFunction>> methods)
    : name{std::move(name)}, _superclass{superclass}, _methods{std::move(methods)}
{}

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

std::string LoxClass::toString() const
{
    return name;
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
    auto instance = std::make_shared<LoxInstance>(shared_from_this());

    if (auto initializer = findMethod("init")) {
        initializer->bind(instance)->call(args);
    }

    return instance;
}

void LoxClass::enumTraceables(Traceable::Enumerator const& enumerator)
{
    if (_superclass) {
        enumerator.enumerate(*_superclass);
    }

    for (auto& [_, method] : _methods) {
        enumerator.enumerate(*method);
    }
}

void LoxClass::reclaimTraceables()
{
    if (_superclass) {
        _superclass->reclaimTraceables();
        _superclass.reset();
    }

    for (auto& [_, method] : _methods) {
        method->reclaimTraceables();
    }
    _methods.clear();
}

} // namespace cloxx
