#pragma once

#include "LoxInstance.hpp"

namespace cloxx {

class LoxClass;
class Interpreter;

class LoxNumberInstance : public LoxInstance {
public:
    using LoxInstance::LoxInstance;

    std::string toString() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    double value;
};

std::shared_ptr<LoxClass> createNumberClass(Interpreter* interpreter);

} // namespace cloxx
