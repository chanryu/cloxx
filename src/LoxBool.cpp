#include "LoxBool.hpp"

#include "Assert.hpp"
#include "Interpreter.hpp"
#include "LoxClass.hpp"
#include "LoxInstance.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"

namespace cloxx {

namespace {

class LoxBoolClass : public LoxClass {
public:
    using LoxClass::LoxClass;

    std::shared_ptr<LoxInstance> createInstance(std::shared_ptr<LoxClass> const& klass) override
    {
        return _interpreter->create<LoxBoolInstance>(klass);
    }
};

std::map<std::string, std::shared_ptr<LoxFunction>> createBoolMethods(Interpreter* /*interpreter*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

bool LoxBoolInstance::isTruthy()
{
    return value;
}

std::string LoxBoolInstance::toString()
{
    return value ? "true" : "false";
}

bool LoxBoolInstance::equals(std::shared_ptr<LoxObject> const& object)
{
    auto other = std::dynamic_pointer_cast<LoxBoolInstance>(object);
    return other && value == other->value;
}

std::shared_ptr<LoxClass> createBoolClass(Interpreter* interpreter)
{
    return interpreter->create<LoxBoolClass>(interpreter, "Bool", /*superclass*/ nullptr,
                                             createBoolMethods(interpreter));
}

} // namespace cloxx
