#pragma once

#include <memory>

#include "Callable.hpp"
#include "LoxObject.hpp"

namespace cloxx {

class Runtime;
class LoxClass;

class LoxFunction : public LoxObject, public Callable {
public:
    LoxFunction(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass);

    virtual std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxObject> const& instance) const = 0;
};

std::shared_ptr<LoxClass> createFunctionClass(Runtime* runtime);

} // namespace cloxx
