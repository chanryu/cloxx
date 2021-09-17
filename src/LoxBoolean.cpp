#include "LoxBoolean.hpp"

namespace cloxx {

LoxBoolean::LoxBoolean(bool value) : value{value}
{}

std::string LoxBoolean::toString()
{
    return value ? "true" : "false";
}

bool LoxBoolean::isTruthy()
{
    return value;
}

bool LoxBoolean::equals(std::shared_ptr<LoxObject> const& object)
{
    if (LoxObject::equals(object)) {
        return true;
    }

    if (auto b = dynamic_cast<LoxBoolean const*>(object.get())) {
        return value == b->value;
    }

    return false;
}

} // namespace cloxx
