#pragma once

#include <memory>

namespace cloxx {

class LoxClass;
class GarbageCollector;

std::shared_ptr<LoxClass> createListClass(GarbageCollector* gc);

} // namespace cloxx
