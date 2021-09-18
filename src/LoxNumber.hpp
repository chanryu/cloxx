#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxNumber : public LoxObject {
public:
    using ValueType = double;

    explicit LoxNumber(ValueType value);

    std::string toString() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    ValueType const value;
};

template <typename T>
auto toLoxNumber(T value)
{
    return std::make_shared<LoxNumber>(value);
}

} // namespace cloxx
