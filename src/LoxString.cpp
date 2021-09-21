#include "LoxString.hpp"

#include "LoxClass.hpp"
#include "Runtime.hpp"

namespace cloxx {

std::string LoxString::toString()
{
    return value;
}

bool LoxString::equals(std::shared_ptr<LoxObject> const& object)
{
    if (LoxObject::equals(object)) {
        return true;
    }

    if (auto str = dynamic_cast<LoxString const*>(object.get())) {
        return value == str->value;
    }

    return false;
}

namespace {

std::map<std::string, std::shared_ptr<LoxFunction>> createStringMethods(Runtime* /*runtime*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createStringClass(Runtime* runtime)
{
    return runtime->create<LoxClass>("String", runtime->objectClass(), createStringMethods(runtime),
                                     [runtime](auto const& klass) {
                                         return runtime->create<LoxString>(klass);
                                     });
}

} // namespace cloxx
