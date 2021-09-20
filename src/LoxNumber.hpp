#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxClass;
class Interpreter;

class LoxNumber : public LoxObject {
public:
    using LoxObject::LoxObject;

    std::string toString() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    double value;
};

std::shared_ptr<LoxClass> createNumberClass(Interpreter* interpreter);

} // namespace cloxx
