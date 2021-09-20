#include "LoxFunction.hpp"

#include "Interpreter.hpp"
#include "LoxClass.hpp"

namespace cloxx {

LoxFunction::LoxFunction(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass) : LoxObject{tag, klass}
{}

std::shared_ptr<LoxClass> createFunctionClass(Interpreter* interpreter)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> funcClassMethods;
    return interpreter->create<LoxClass>(interpreter, "Function", interpreter->objectClass(), funcClassMethods);
}

} // namespace cloxx
