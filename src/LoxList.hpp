#pragma once

#include <memory>

namespace cloxx {

class LoxClass;
class Runtime;

std::shared_ptr<LoxClass> createListClass(Runtime* runtime);

} // namespace cloxx
