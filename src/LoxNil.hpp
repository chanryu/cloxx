#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxNil : public LoxObject {
public:
    std::string toString() const override;
    bool isTruthy() const override;
    bool equals(LoxObject const& object) const override;
};

inline auto makeLoxNil()
{
    return std::make_shared<LoxNil>();
}

} // namespace cloxx
