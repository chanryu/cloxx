#pragma once

#include <memory>

#include "Callable.hpp"
#include "LoxInstance.hpp"

namespace cloxx {

class Interpreter;
class LoxClass;

class LoxFunction : public LoxInstance, public Callable {
public:
    LoxFunction(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass);

    virtual std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> const& instance) const = 0;
};

std::shared_ptr<LoxClass> createFunctionClass(Interpreter* interpreter);

} // namespace cloxx
