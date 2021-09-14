#include "LoxString.hpp"

namespace cloxx {

LoxString::LoxString(std::string value) : value{std::move(value)}
{}

std::string LoxString::toString()
{
    return value;
}

bool LoxString::equals(LoxObject const& object) const
{
    if (LoxObject::equals(object)) {
        return true;
    }

    if (auto str = dynamic_cast<LoxString const*>(&object)) {
        return value == str->value;
    }

    return false;
}

} // namespace cloxx
