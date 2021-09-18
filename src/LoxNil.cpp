#include "LoxBool.hpp"

#include "Assert.hpp"
#include "Interpreter.hpp"
#include "LoxClass.hpp"
#include "LoxInstance.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"
#include "LoxString.hpp"

namespace cloxx {

namespace {

class NilInstance : public LoxInstance {
public:
    using LoxInstance::LoxInstance;

    bool isTruthy() override
    {
        return false;
    }
};

std::map<std::string, std::shared_ptr<LoxFunction>> createNilMethods(Interpreter* interpreter, LoxClass* /*klass*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    methods.emplace("toString", interpreter->create<LoxNativeFunction>(interpreter, /*arity*/ 0,
                                                                       [](auto& /*instance*/, auto& /*args*/) {
                                                                           return toLoxString("nil");
                                                                       }));
    methods.emplace("equals", interpreter->create<LoxNativeFunction>(
                                  interpreter, /*arity*/ 1, [interpreter](auto& /*instance*/, auto& args) {
                                      LOX_ASSERT(args.size() == 1);
                                      return interpreter->toLoxBool(!!std::dynamic_pointer_cast<NilInstance>(args[0]));
                                  }));
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createNilClass(Interpreter* interpreter)
{
    auto methodFactory = [interpreter](LoxClass* klass) {
        return createNilMethods(interpreter, klass);
    };

    return interpreter->create<LoxClass>(interpreter, "Nil", /*superclass*/ nullptr, std::move(methodFactory),
                                         /*dataFactory*/ nullptr);
}

std::shared_ptr<LoxInstance> createNilInstance(Interpreter* interpreter, std::shared_ptr<LoxClass> const& klass)
{
    return interpreter->create<NilInstance>(klass);
}

} // namespace cloxx
