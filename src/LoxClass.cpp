#include "LoxClass.hpp"

#include "Assert.hpp"
#include "LoxFunction.hpp"
#include "Runtime.hpp"
#include "RuntimeError.hpp"

namespace cloxx {

LoxClass::LoxClass(PrivateCreationTag tag, Runtime* runtime, std::string name,
                   std::shared_ptr<LoxClass> const& superclass,
                   std::map<std::string, std::shared_ptr<LoxObject>> fields,
                   std::map<std::string, std::shared_ptr<LoxFunction>> methods, LoxObjectFactory objectFactory)
    : LoxObject{tag}, _runtime{runtime}, _name{std::move(name)},
      _superclass{superclass}, _fields{std::move(fields)}, _methods{std::move(methods)}, _objectFactory{objectFactory}
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

    populateInstanceFields(instance->_fields);

    if (auto initializer = findMethod("init")) {
        initializer->bind(instance)->call(args);
    }

    return instance;
}

void LoxClass::enumerateTraceables(Traceable::Enumerator const& enumerator)
{
    LoxObject::enumerateTraceables(enumerator);

    if (_superclass) {
        enumerator.enumerate(*_superclass);
    }

    for (auto& [_, method] : _methods) {
        enumerator.enumerate(*method);
    }
}

void LoxClass::reclaim()
{
    LoxObject::reclaim();

    _superclass.reset();
    _methods.clear();
}

std::shared_ptr<LoxObject> LoxClass::createInstance(std::shared_ptr<LoxClass> const& klass)
{
    if (_objectFactory) {
        return _objectFactory(klass);
    }

    if (_superclass) {
        return _superclass->createInstance(klass);
    }

    return _runtime->create<LoxObject>(klass);
}

void LoxClass::populateInstanceFields(std::map<std::string, std::shared_ptr<LoxObject>>& fields)
{
    if (_superclass) {
        _superclass->populateInstanceFields(fields);
    }

    for (auto const& [name, value] : _fields) {
        fields[name] = value; // overwrite if already exist
    }
}

} // namespace cloxx
