#include "LoxFunction.hpp"

#include "Interpreter.hpp"
#include "LoxObject.hpp"

namespace cloxx {

LoxFunction::LoxFunction(PrivateCreationTag tag, std::shared_ptr<LoxObjectClass> const& klass) : LoxObject{tag, klass}
{}

std::shared_ptr<LoxObjectClass> createFunctionClass(Interpreter* interpreter)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> funcClassMethods;
    return interpreter->create<LoxObjectClass>(interpreter, "Function", /*superclass*/ nullptr, funcClassMethods);
}

} // namespace cloxx
