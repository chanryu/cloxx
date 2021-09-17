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

bool LoxObject::equals(std::shared_ptr<LoxObject> const& object)
{
    return this == object.get();
}

} // namespace cloxx
