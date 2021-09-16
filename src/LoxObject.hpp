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

    virtual std::string toString() = 0;
    virtual bool isTruthy();
    virtual bool equals(LoxObject const& object) const;
};

} // namespace cloxx
