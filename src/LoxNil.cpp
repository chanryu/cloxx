#include "LoxBool.hpp"

#include "Assert.hpp"
#include "Interpreter.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"
#include "LoxObject.hpp"
#include "LoxString.hpp"

namespace cloxx {

namespace {

class LoxNilInstance : public LoxObject {
public:
    using LoxObject::LoxObject;

    std::string toString() override
    {
        return "nil";
    }

    bool isTruthy() override
    {
        return false;
    }

    bool equals(std::shared_ptr<LoxObject> const& object) override
    {
        return !!std::dynamic_pointer_cast<LoxNilInstance>(object);
    }
};

class LoxNilClass : public LoxObjectClass {
public:
    using LoxObjectClass::LoxObjectClass;

    std::shared_ptr<LoxObject> createInstance(std::shared_ptr<LoxObjectClass> const& klass) override
    {
        return _interpreter->create<LoxNilInstance>(klass);
    }
};

std::map<std::string, std::shared_ptr<LoxFunction>> createNilMethods(Interpreter* /*interpreter*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

std::shared_ptr<LoxObjectClass> createNilClass(Interpreter* interpreter)
{
    return interpreter->create<LoxNilClass>(interpreter, "Nil", /*superclass*/ nullptr, createNilMethods(interpreter));
}

} // namespace cloxx
