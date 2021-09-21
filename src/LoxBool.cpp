#include "LoxBool.hpp"

#include "Assert.hpp"
#include "Runtime.hpp"

#include "LoxClass.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"

namespace cloxx {

namespace {

std::map<std::string, std::shared_ptr<LoxFunction>> createBoolMethods(Runtime* /*runtime*/)
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

std::shared_ptr<LoxClass> createBoolClass(Runtime* runtime)
{
    return runtime->create<LoxClass>("Bool", runtime->objectClass(), createBoolMethods(runtime),
                                     [runtime](auto const& klass) {
                                         return runtime->create<LoxBool>(klass);
                                     });
}

} // namespace cloxx
