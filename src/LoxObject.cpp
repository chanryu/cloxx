#include "LoxObject.hpp"

namespace cloxx {

// LoxObject

#ifdef CLOXX_GC_DEBUG
namespace {
size_t objectInstanceCount = 0;
}

LoxObject::LoxObject()
{
    objectInstanceCount += 1;
}

LoxObject::~LoxObject()
{
    objectInstanceCount -= 1;
}

size_t LoxObject::instanceCount()
{
    return objectInstanceCount;
}
#endif

bool LoxObject::isTruthy() const
{
    return true;
}

bool LoxObject::equals(LoxObject const& object) const
{
    return this == &object;
}

// LoxNil

std::string LoxNil::toString() const
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

// LoxNumber

LoxNumber::LoxNumber(double value) : value{value}
{}

std::string LoxNumber::toString() const
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

bool LoxNumber::equals(LoxObject const& object) const
{
    if (LoxObject::equals(object)) {
        return true;
    }

    if (auto num = dynamic_cast<LoxNumber const*>(&object)) {
        return value == num->value;
    }

    return false;
}

// LoxString

LoxString::LoxString(std::string value) : value{std::move(value)}
{}

std::string LoxString::toString() const
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

// LoxBoolean

LoxBoolean::LoxBoolean(bool value) : value{value}
{}

std::string LoxBoolean::toString() const
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
