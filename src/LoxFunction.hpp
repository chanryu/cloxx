#pragma once

#include <memory>

#include "Callable.hpp"
#include "LoxObject.hpp"

namespace cloxx {

class Interpreter;
class LoxObjectClass;

class LoxFunction : public LoxObject, public Callable {
public:
    LoxFunction(PrivateCreationTag tag, std::shared_ptr<LoxObjectClass> const& klass);

    virtual std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxObject> const& instance) const = 0;
};

std::shared_ptr<LoxObjectClass> createFunctionClass(Interpreter* interpreter);

} // namespace cloxx
