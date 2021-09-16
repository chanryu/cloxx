#pragma once

#include <memory>

namespace cloxx {

class LoxClass;
class GarbageCollector;

std::shared_ptr<LoxClass> createBoolClass(GarbageCollector* gc);

} // namespace cloxx
