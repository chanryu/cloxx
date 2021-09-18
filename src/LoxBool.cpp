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

class BoolInstance : public LoxInstance {
public:
    BoolInstance(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass, bool value)
        : LoxInstance{tag, klass}, _value{value}
    {}

    bool isTruthy() override
    {
        return _value;
    }

private:
    bool _value;
};

std::map<std::string, std::shared_ptr<LoxFunction>> createBoolMethods(Interpreter* interpreter, LoxClass* klass)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    methods.emplace("toString", interpreter->create<LoxNativeFunction>(
                                    interpreter, /*arity*/ 0, [](auto& instance, auto& /*args*/) {
                                        return toLoxString(instance->isTruthy() ? "true" : "false");
                                    }));
    methods.emplace("equals", interpreter->create<LoxNativeFunction>(
                                  interpreter, /*arity*/ 1, [interpreter, klass](auto& instance, auto& args) {
                                      LOX_ASSERT(args.size() == 1);
                                      auto other = std::dynamic_pointer_cast<LoxInstance>(args[0]);
                                      if (!other || other->klass().get() != klass) {
                                          return interpreter->toLoxBool(false);
                                      }
                                      return interpreter->toLoxBool(instance->isTruthy() == other->isTruthy());
                                  }));
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createBoolClass(Interpreter* interpreter)
{
    auto methodFactory = [interpreter](LoxClass* klass) {
        return createBoolMethods(interpreter, klass);
    };

    return interpreter->create<LoxClass>(interpreter, "Bool", /*superclass*/ nullptr, std::move(methodFactory),
                                         /*dataFactory*/ nullptr);
}

std::shared_ptr<LoxInstance> createBoolInstance(Interpreter* interpreter, std::shared_ptr<LoxClass> const& klass,
                                                bool value)
{
    return interpreter->create<BoolInstance>(klass, value);
}

} // namespace cloxx
