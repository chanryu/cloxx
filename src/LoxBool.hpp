#pragma once

#include <memory>

namespace cloxx {

class LoxClass;
class LoxInstance;
class GarbageCollector;

std::shared_ptr<LoxClass> createBoolClass(GarbageCollector* gc);
std::shared_ptr<LoxInstance> createBoolInstance(GarbageCollector* gc);

} // namespace cloxx
