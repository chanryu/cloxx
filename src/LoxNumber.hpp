#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxNumber : public LoxObject {
public:
    using ValueType = double;

    explicit LoxNumber(ValueType value);

    std::string toString() override;
    bool equals(LoxObject const& object) const override;

    ValueType const value;
};

template <typename T>
auto toLoxNumber(T value)
{
    return std::make_shared<LoxNumber>(value);
}

} // namespace cloxx
