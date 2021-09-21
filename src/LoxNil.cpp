#include "LoxBool.hpp"

#include "Assert.hpp"
#include "Runtime.hpp"

#include "LoxClass.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"
#include "LoxString.hpp"

namespace cloxx {

namespace {

class LoxNil : public LoxObject {
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
        return !!std::dynamic_pointer_cast<LoxNil>(object);
    }
};

std::map<std::string, std::shared_ptr<LoxFunction>> createNilMethods(Runtime* /*runtime*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createNilClass(Runtime* runtime)
{
    return runtime->create<LoxClass>("Nil", runtime->objectClass(), createNilMethods(runtime),
                                     [runtime](auto const& klass) {
                                         return runtime->create<LoxNil>(klass);
                                     });
}

} // namespace cloxx
