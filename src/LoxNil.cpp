#include "LoxNil.hpp"

namespace cloxx {

std::string LoxNil::toString()
{
    return "nil";
}

bool LoxNil::isTruthy() const
{
    return false;
}

bool LoxNil::equals(LoxObject const& object) const
{
    if (LoxObject::equals(object)) {
        return true;
    }

    return dynamic_cast<LoxNil const*>(&object) != nullptr;
}

} // namespace cloxx
