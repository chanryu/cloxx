#pragma once

#include <memory>

namespace cloxx {

class LoxClass;
class LoxInstance;
class Interpreter;

std::shared_ptr<LoxClass> createBoolClass(Interpreter* interpreter);
std::shared_ptr<LoxInstance> createBoolInstance(Interpreter* interpreter, std::shared_ptr<LoxClass> const& boolClass,
                                                bool value);

} // namespace cloxx
