#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxClass;
class Runtime;

class LoxBool : public LoxObject {
public:
    using LoxObject::LoxObject;

    std::string toString() override;
    bool isTruthy() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    bool value;
};

std::shared_ptr<LoxClass> createBoolClass(Runtime* runtime);

} // namespace cloxx
