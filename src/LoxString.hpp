#pragma once

#include "LoxClass.hpp"
#include "LoxInstance.hpp"

namespace cloxx {

class LoxStringInstance : public LoxInstance {
public:
    using LoxInstance::LoxInstance;

    std::string toString() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    std::string value;
};

class LoxStringClass : public LoxClass {
public:
    using LoxClass::LoxClass;

    std::shared_ptr<LoxInstance> createInstance(std::shared_ptr<LoxClass> const& klass) override;
};

std::shared_ptr<LoxClass> createStringClass(Interpreter* interpreter);

} // namespace cloxx
