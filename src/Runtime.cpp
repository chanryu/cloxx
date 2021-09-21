#include "Runtime.hpp"

#include "Assert.hpp"
#include "Environment.hpp"

#include "LoxBool.hpp"
#include "LoxClass.hpp"
#include "LoxFunction.hpp"
#include "LoxList.hpp"
#include "LoxNil.hpp"
#include "LoxNumber.hpp"
#include "LoxString.hpp"

namespace cloxx {

Runtime::Runtime()
{
    _listClass = createListClass(this);

    // Built-in classes - Object, Class, Function, Nil
    _gc.root()->define("List", _listClass);
}

std::shared_ptr<Environment> const& Runtime::root()
{
    return _gc.root();
}

void Runtime::collectGarbage()
{
    _gc.collect();
}

std::shared_ptr<LoxClass> const& Runtime::objectClass()
{
    if (!_objectClass) {
        _objectClass = createObjectClass(this);
    }
    return _objectClass;
}

std::shared_ptr<LoxClass> const& Runtime::functionClass()
{
    if (!_functionClass) {
        _functionClass = createFunctionClass(this);
    }
    return _functionClass;
}

std::shared_ptr<LoxObject> Runtime::getNil()
{
    if (!_nil) {
        auto nilClass = createNilClass(this);
        _nil = nilClass->call({});
    }
    return _nil;
}

std::shared_ptr<LoxObject> Runtime::toLoxBool(bool value)
{
    if (!_boolClass) {
        _boolClass = createBoolClass(this);
    }

    auto& instance = value ? _true : _false;
    if (!instance) {
        instance = _boolClass->call({});
        static_cast<LoxBool*>(instance.get())->value = value;
    }
    return instance;
}

std::shared_ptr<LoxObject> Runtime::toLoxNumber(double value)
{
    if (!_numberClass) {
        _numberClass = createNumberClass(this);
    }
    auto instance = _numberClass->call({});
    static_cast<LoxNumber*>(instance.get())->value = value;
    return instance;
}

std::shared_ptr<LoxObject> Runtime::toLoxString(std::string value)
{
    if (!_stringClass) {
        _stringClass = createStringClass(this);
    }
    auto instance = _stringClass->call({});
    static_cast<LoxString*>(instance.get())->value = std::move(value);
    return instance;
}

} // namespace cloxx