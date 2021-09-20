#pragma once

#include <memory>

namespace cloxx {

class LoxObjectClass;
class LoxObject;
class Interpreter;

std::shared_ptr<LoxObjectClass> createNilClass(Interpreter* interpreter);

} // namespace cloxx
