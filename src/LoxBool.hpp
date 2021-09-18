#pragma once

#include <memory>

namespace cloxx {

class LoxClass;
class LoxInstance;
class Interpreter;

std::shared_ptr<LoxClass> createBoolClass(Interpreter* interpreter);

} // namespace cloxx
