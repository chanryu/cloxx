#include "LoxModule.hpp"

#include "Assert.hpp"
#include "Environment.hpp"
#include "Runtime.hpp"

#include "LoxClass.hpp"

namespace cloxx {

namespace {

auto createModuleFields(Runtime* /*runtime*/)
{
    std::map<std::string, std::shared_ptr<LoxObject>> fields;
    // no fields yet
    return fields;
}

auto createModuleMethods(Runtime* /*runtime*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

std::string LoxModule::toString()
{
    return "Module";
}

void LoxModule::enumerateTraceables(Enumerator const& enumerator)
{
    LoxObject::enumerateTraceables(enumerator);

    LOX_ASSERT(env);
    enumerator.enumerate(*env);
}

void LoxModule::reclaim()
{
    LoxObject::reclaim();

    env.reset();
}

std::shared_ptr<LoxClass> createModuleClass(Runtime* runtime)
{
    return runtime->create<LoxClass>("Module", runtime->objectClass(), createModuleFields(runtime),
                                     createModuleMethods(runtime), [runtime](auto const& klass) {
                                         return runtime->create<LoxModule>(klass);
                                     });
}

} // namespace cloxx