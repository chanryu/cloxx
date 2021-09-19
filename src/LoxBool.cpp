#include "LoxBool.hpp"

#include "Assert.hpp"
#include "Interpreter.hpp"
#include "LoxClass.hpp"
#include "LoxInstance.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"
#include "LoxString.hpp"

namespace cloxx {

namespace {

class LoxBoolInstance : public LoxInstance {
public:
    using LoxInstance::LoxInstance;

    bool isTruthy() override
    {
        return value;
    }

    std::string toString() override
    {
        return value ? "true" : "false";
    }

    bool equals(std::shared_ptr<LoxObject> const& object) override
    {
        auto other = std::dynamic_pointer_cast<LoxBoolInstance>(object);
        return other && value == other->value;
    }

    bool value;
};

class LoxBoolClass : public LoxClass {
public:
    using LoxClass::LoxClass;

    std::shared_ptr<LoxInstance> createInstance(std::shared_ptr<LoxClass> const& klass) override
    {
        return _interpreter->create<LoxBoolInstance>(klass);
    }
};

auto toBoolInstance(std::shared_ptr<LoxInstance> const& instance)
{
    LOX_ASSERT(std::dynamic_pointer_cast<LoxBoolInstance>(instance));
    return static_cast<LoxBoolInstance*>(instance.get());
}

std::map<std::string, std::shared_ptr<LoxFunction>> createBoolMethods(Interpreter* /*interpreter*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createBoolClass(Interpreter* interpreter)
{
    return interpreter->create<LoxBoolClass>(interpreter, "Bool", /*superclass*/ nullptr,
                                             createBoolMethods(interpreter));
}

std::shared_ptr<LoxInstance> createBoolInstance(std::shared_ptr<LoxClass> const& klass, bool value)
{
    auto instance = std::static_pointer_cast<LoxInstance>(klass->call({}));
    toBoolInstance(instance)->value = value;
    return instance;
}

} // namespace cloxx
