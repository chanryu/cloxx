#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxNil : public LoxObject {
public:
    std::string toString() override;
    bool isTruthy() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;
};

inline auto makeLoxNil()
{
    return std::make_shared<LoxNil>();
}

} // namespace cloxx
