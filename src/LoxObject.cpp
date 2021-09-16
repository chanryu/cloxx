#include "LoxObject.hpp"

namespace cloxx {

#ifdef CLOXX_GC_DEBUG
namespace {
size_t objectInstanceCount = 0;
}

LoxObject::LoxObject()
{
    objectInstanceCount += 1;
}

LoxObject::~LoxObject()
{
    objectInstanceCount -= 1;
}

size_t LoxObject::instanceCount()
{
    return objectInstanceCount;
}
#endif

bool LoxObject::isTruthy()
{
    return true;
}

bool LoxObject::equals(LoxObject const& object) const
{
    return this == &object;
}

} // namespace cloxx
