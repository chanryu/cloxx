#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxObjectClass;
class Interpreter;

class LoxNumber : public LoxObject {
public:
    using LoxObject::LoxObject;

    std::string toString() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    double value;
};

std::shared_ptr<LoxObjectClass> createNumberClass(Interpreter* interpreter);

} // namespace cloxx
