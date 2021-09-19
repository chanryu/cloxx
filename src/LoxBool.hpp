#pragma once

#include "LoxInstance.hpp"

namespace cloxx {

class LoxClass;
class Interpreter;

class LoxBoolInstance : public LoxInstance {
public:
    using LoxInstance::LoxInstance;

    std::string toString() override;
    bool isTruthy() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    bool value;
};

std::shared_ptr<LoxClass> createBoolClass(Interpreter* interpreter);

} // namespace cloxx
