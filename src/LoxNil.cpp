#include "LoxNil.hpp"

namespace cloxx {

std::string LoxNil::toString()
{
    return "nil";
}

bool LoxNil::isTruthy()
{
    return false;
}

bool LoxNil::equals(std::shared_ptr<LoxObject> const& object)
{
    if (LoxObject::equals(object)) {
        return true;
    }

    return dynamic_cast<LoxNil const*>(object.get()) != nullptr;
}

} // namespace cloxx
