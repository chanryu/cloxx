#include "LoxBool.hpp"

#include "Assert.hpp"
#include "Interpreter.hpp"
#include "LoxClass.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"

namespace cloxx {

namespace {

class LoxBoolClass : public LoxClass {
public:
    using LoxClass::LoxClass;

    std::shared_ptr<LoxObject> createInstance(std::shared_ptr<LoxClass> const& klass) override
    {
        return _interpreter->create<LoxBool>(klass);
    }
};

std::map<std::string, std::shared_ptr<LoxFunction>> createBoolMethods(Interpreter* /*interpreter*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

bool LoxBool::isTruthy()
{
    return value;
}

std::string LoxBool::toString()
{
    return value ? "true" : "false";
}

bool LoxBool::equals(std::shared_ptr<LoxObject> const& object)
{
    auto other = std::dynamic_pointer_cast<LoxBool>(object);
    return other && value == other->value;
}

std::shared_ptr<LoxClass> createBoolClass(Interpreter* interpreter)
{
    return interpreter->create<LoxBoolClass>(interpreter, "Bool", interpreter->objectClass(),
                                             createBoolMethods(interpreter));
}

} // namespace cloxx
