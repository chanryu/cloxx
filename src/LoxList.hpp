#pragma once

#include <memory>

namespace cloxx {

class LoxObjectClass;
class Interpreter;

std::shared_ptr<LoxObjectClass> createListClass(Interpreter* interpreter);

} // namespace cloxx
