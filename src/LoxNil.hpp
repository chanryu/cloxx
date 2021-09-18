#pragma once

#include <memory>

namespace cloxx {

class LoxClass;
class LoxInstance;
class Interpreter;

std::shared_ptr<LoxClass> createNilClass(Interpreter* interpreter);
std::shared_ptr<LoxInstance> createNilInstance(Interpreter* interpreter, std::shared_ptr<LoxClass> const& nilClass);

} // namespace cloxx
