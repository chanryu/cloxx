#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxBoolean : public LoxObject {
public:
    explicit LoxBoolean(bool value);

    std::string toString() override;
    bool isTruthy() override;
    bool equals(LoxObject const& object) const override;

    bool const value;
};

inline auto toLoxBoolean(bool value)
{
    return std::make_shared<LoxBoolean>(value);
}

} // namespace cloxx
