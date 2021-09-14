#include "LoxNumber.hpp"

namespace cloxx {

LoxNumber::LoxNumber(ValueType value) : value{value}
{}

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

} // namespace cloxx
