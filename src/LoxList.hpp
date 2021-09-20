#pragma once

#include <memory>

namespace cloxx {

class LoxClass;
class Interpreter;

std::shared_ptr<LoxClass> createListClass(Interpreter* interpreter);

} // namespace cloxx
