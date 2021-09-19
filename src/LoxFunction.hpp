#pragma once

#include <memory>

#include "Callable.hpp"
#include "GC.hpp"
#include "LoxObject.hpp"

namespace cloxx {

class LoxInstance;

class LoxFunction : public LoxObject, public Callable {
public:
    LoxFunction(PrivateCreationTag tag);

    virtual std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> const& instance) const = 0;
};

} // namespace cloxx
