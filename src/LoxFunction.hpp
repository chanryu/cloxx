#pragma once

#include <memory>

#include "Callable.hpp"
#include "GC.hpp"
#include "LoxObject.hpp"

namespace cloxx {

enum class LoxClassId : size_t;

class LoxInstance;

class LoxFunction : public LoxObject, public Callable, public Traceable {
public:
    LoxFunction(PrivateCreationTag tag);

    virtual std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> const& instance) const = 0;

    // Move this to LoxMethod
    virtual void setClassId(LoxClassId /*classId*/)
    {
        // NOOP
    }
};

} // namespace cloxx
