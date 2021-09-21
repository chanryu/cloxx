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
    _objectClass = createObjectClass(this);
    _functionClass = createFunctionClass(this);
    _nilClass = createNilClass(this);
    _boolClass = createBoolClass(this);
    _numberClass = createNumberClass(this);
    _stringClass = createStringClass(this);
    _listClass = createListClass(this);

    // Built-in classes - Object, Class, Function, Nil
    _gc.root()->define("Object", _objectClass);
    _gc.root()->define("Function", _functionClass);
    _gc.root()->define("Nil", _nilClass);
    _gc.root()->define("Bool", _boolClass);
    _gc.root()->define("Number", _numberClass);
    _gc.root()->define("String", _stringClass);
    _gc.root()->define("List", _listClass);
}

std::shared_ptr<LoxClass> Runtime::objectClass()
{
    LOX_ASSERT(_objectClass);
    return _objectClass;
}

std::shared_ptr<LoxClass> Runtime::functionClass()
{
    LOX_ASSERT(_functionClass);
    return _functionClass;
}

std::shared_ptr<LoxObject> Runtime::makeLoxNil()
{
    LOX_ASSERT(_nilClass);
    return _nilClass->call({});
}

std::shared_ptr<LoxObject> Runtime::toLoxBool(bool value)
{
    LOX_ASSERT(_boolClass);
    auto instance = _boolClass->call({});
    static_cast<LoxBool*>(instance.get())->value = value;
    return instance;
}

std::shared_ptr<LoxObject> Runtime::toLoxNumber(double value)
{
    LOX_ASSERT(_numberClass);
    auto instance = _numberClass->call({});
    static_cast<LoxNumber*>(instance.get())->value = value;
    return instance;
}

std::shared_ptr<LoxObject> Runtime::toLoxString(std::string value)
{
    LOX_ASSERT(_stringClass);
    auto instance = _stringClass->call({});
    static_cast<LoxString*>(instance.get())->value = std::move(value);
    return instance;
}

} // namespace cloxx