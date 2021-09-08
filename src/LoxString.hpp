#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxString : public LoxObject {
public:
    explicit LoxString(std::string value);

    std::string toString() const override;
    bool equals(LoxObject const& object) const override;

    std::string const value;
};

inline auto toLoxString(std::string value)
{
    return std::make_shared<LoxString>(std::move(value));
}

} // namespace cloxx
