#pragma once

#include <memory>
#include <string>

namespace cloxx {

class LoxObject {
public:
#ifdef CLOXX_GC_DEBUG
    LoxObject();
    virtual ~LoxObject();

    static size_t instanceCount();
#else
    virtual ~LoxObject() = default;
#endif

    virtual std::string toString() const = 0;
    virtual bool isTruthy() const;
    virtual bool equals(LoxObject const& object) const;
};

} // namespace cloxx
