#pragma once

#include <memory>

namespace cloxx {

class Runtime;
class LoxClass;

std::shared_ptr<LoxClass> createNilClass(Runtime* runtime);

} // namespace cloxx
