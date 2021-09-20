#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxObjectClass;
class Interpreter;

class LoxString : public LoxObject {
public:
    using LoxObject::LoxObject;

    std::string toString() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    std::string value;
};

std::shared_ptr<LoxObjectClass> createStringClass(Interpreter* interpreter);

} // namespace cloxx
