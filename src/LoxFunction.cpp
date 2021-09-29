#include "LoxFunction.hpp"

#include "LoxClass.hpp"
#include "Runtime.hpp"

namespace cloxx {

auto createFunctionFields(Runtime* /*runtime*/)
{
    std::map<std::string, std::shared_ptr<LoxObject>> fields;
    // no fields yet
    return fields;
}

auto createFunctionMethods(Runtime* /*runtime*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

LoxFunction::LoxFunction(PrivateCreationTag tag, Runtime* runtime) : LoxObject{tag, runtime, runtime->functionClass()}
{}

std::shared_ptr<LoxClass> createFunctionClass(Runtime* runtime)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> funcClassMethods;
    return runtime->create<LoxClass>("Function", runtime->objectClass(), createFunctionFields(runtime),
                                     createFunctionMethods(runtime), /*objectFactory*/ nullptr);
}

} // namespace cloxx
