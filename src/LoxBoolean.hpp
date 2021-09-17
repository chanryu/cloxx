#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxBoolean : public LoxObject {
public:
    explicit LoxBoolean(bool value);

    std::string toString() override;
    bool isTruthy() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    bool const value;
};

inline auto toLoxBoolean(bool value)
{
    return std::make_shared<LoxBoolean>(value);
}

} // namespace cloxx
