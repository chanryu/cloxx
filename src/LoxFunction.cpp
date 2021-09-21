#include "LoxFunction.hpp"

#include "LoxClass.hpp"
#include "Runtime.hpp"

namespace cloxx {

LoxFunction::LoxFunction(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass) : LoxObject{tag, klass}
{}

std::shared_ptr<LoxClass> createFunctionClass(Runtime* runtime)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> funcClassMethods;
    return runtime->create<LoxClass>(runtime, "Function", runtime->objectClass(), funcClassMethods);
}

} // namespace cloxx
