#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxNumber : public LoxObject {
public:
    explicit LoxNumber(double value);

    std::string toString() const override;
    bool equals(LoxObject const& object) const override;

    double const value;
};

inline auto toLoxNumber(double value)
{
    return std::make_shared<LoxNumber>(value);
}

} // namespace cloxx
