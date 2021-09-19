#pragma once

#include "LoxInstance.hpp"

namespace cloxx {

class LoxClass;
class Interpreter;

class LoxStringInstance : public LoxInstance {
public:
    using LoxInstance::LoxInstance;

    std::string toString() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    std::string value;
};

std::shared_ptr<LoxClass> createStringClass(Interpreter* interpreter);

} // namespace cloxx
