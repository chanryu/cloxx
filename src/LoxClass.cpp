#include "LoxClass.hpp"

#include "Interpreter.hpp"
#include "LoxFunction.hpp"

namespace cloxx {

namespace {
auto createClassFields(LoxClass* /*klass*/)
{
    std::map<std::string, std::shared_ptr<LoxObject>> fields;
    // TODO: new()
    return fields;
}
} // namespace

LoxClass::LoxClass(PrivateCreationTag tag, Interpreter* interpreter, std::string name,
                   std::shared_ptr<LoxClass> const& superclass,
                   std::map<std::string, std::shared_ptr<LoxFunction>> methods)
    : LoxInstance{tag, createClassFields(this)}, _interpreter{interpreter}, _name{std::move(name)},
      _superclass{superclass}, _methods{std::move(methods)}
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
    auto instance = createInstance(std::static_pointer_cast<LoxClass>(shared_from_this()));

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

std::shared_ptr<LoxInstance> LoxClass::createInstance(std::shared_ptr<LoxClass> const& klass)
{
    if (_superclass) {
        return _superclass->createInstance(klass);
    }

    return _interpreter->create<LoxInstance>(klass);
}

} // namespace cloxx
