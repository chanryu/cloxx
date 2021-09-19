#pragma once

#include <memory>

namespace cloxx {

class LoxClass;
class LoxObject;
class LoxInstance;
class Interpreter;

std::shared_ptr<LoxClass> createBoolClass(Interpreter* interpreter);
void setBoolValue(std::shared_ptr<LoxObject> const& object, bool value);

} // namespace cloxx
