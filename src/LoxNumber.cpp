#include "LoxNumber.hpp"

#include "LoxClass.hpp"
#include "Runtime.hpp"

namespace cloxx {

std::string LoxNumber::toString()
{
    auto str = std::to_string(value);

    // Remove trailing zeros.
    while (str.size() > 1) {
        char c = str.back();
        if (c == '0') {
            str.pop_back();
        }
        else {
            if (c == '.')
                str.pop_back();
            break;
        }
    }
    return str;
}

bool LoxNumber::equals(std::shared_ptr<LoxObject> const& object)
{
    if (LoxObject::equals(object)) {
        return true;
    }

    if (auto num = dynamic_cast<LoxNumber const*>(object.get())) {
        return value == num->value;
    }

    return false;
}

namespace {

std::map<std::string, std::shared_ptr<LoxFunction>> createNumberMethods(Runtime* /*runtime*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createNumberClass(Runtime* runtime)
{
    return runtime->create<LoxClass>("Number", runtime->objectClass(), createNumberMethods(runtime),
                                     [runtime](auto const& klass) {
                                         return runtime->create<LoxNumber>(klass);
                                     });
}

} // namespace cloxx
