#pragma once

#include <memory>
#include <string>

#include "GC.hpp"

namespace cloxx {

class LoxObject : public Traceable {
public:
    LoxObject(PrivateCreationTag tag);
    virtual ~LoxObject();

#ifdef CLOXX_GC_DEBUG
    static size_t instanceCount();
#endif

    virtual std::string toString() = 0;
    virtual bool isTruthy();
    virtual bool equals(std::shared_ptr<LoxObject> const& object);
};

} // namespace cloxx
