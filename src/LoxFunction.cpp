#include "LoxFunction.hpp"

#include "LoxClass.hpp"
#include "Runtime.hpp"

namespace cloxx {

LoxFunction::LoxFunction(PrivateCreationTag tag, Runtime* runtime) : LoxObject{tag, runtime, runtime->functionClass()}
{}

std::shared_ptr<LoxClass> createFunctionClass(Runtime* runtime)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> funcClassMethods;
    return runtime->create<LoxClass>("Function", runtime->objectClass(), funcClassMethods, /*objectFactory*/ nullptr);
}

} // namespace cloxx
