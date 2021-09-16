#include "LoxBoolean.hpp"

namespace cloxx {

LoxBoolean::LoxBoolean(bool value) : value{value}
{}

std::string LoxBoolean::toString()
{
    return value ? "true" : "false";
}

bool LoxBoolean::isTruthy() const
{
    return value;
}

bool LoxBoolean::equals(LoxObject const& object) const
{
    if (LoxObject::equals(object)) {
        return true;
    }

    if (auto b = dynamic_cast<LoxBoolean const*>(&object)) {
        return value == b->value;
    }

    return false;
}

} // namespace cloxx
