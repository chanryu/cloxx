#include "LoxString.hpp"

namespace cloxx {

LoxString::LoxString(std::string value) : value{std::move(value)}
{}

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

} // namespace cloxx
