#include "LoxBool.hpp"

#include "Assert.hpp"
#include "LoxBoolean.hpp"
#include "LoxClass.hpp"
#include "LoxInstance.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"
#include "LoxString.hpp"

namespace cloxx {

namespace {
struct BoolData : Traceable {
    BoolData(PrivateCreationTag tag) : Traceable{tag}
    {}

    void enumerateTraceables(Enumerator const& /*enumerator*/) override
    {
        // NOOP
    }

    void reclaim() override
    {
        // NOOP
    }

    bool value;
};

auto toBoolNativeData(std::shared_ptr<LoxInstance> const& instance, LoxClass* klass)
{
    auto nativeData = instance->getInstanceData(klass);
    LOX_ASSERT(std::dynamic_pointer_cast<BoolData>(nativeData));
    return static_cast<BoolData*>(nativeData.get());
}

std::map<std::string, std::shared_ptr<LoxFunction>> createBoolMethods(GarbageCollector* gc, LoxClass* klass)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    methods.emplace("init", gc->create<LoxNativeFunction>(gc, /*arity*/ 1, [klass](auto& instance, auto& args) {
        LOX_ASSERT(args.size() == 1);
        toBoolNativeData(instance, klass)->value = args[0]->isTruthy();
        return makeLoxNil();
    }));
    methods.emplace("toString", gc->create<LoxNativeFunction>(gc, /*arity*/ 0, [klass](auto& instance, auto& /*args*/) {
        return toLoxString(toBoolNativeData(instance, klass)->value ? "true" : "false");
    }));
    methods.emplace("isTruthy", gc->create<LoxNativeFunction>(gc, /*arity*/ 0, [klass](auto& instance, auto& /*args*/) {
        return toLoxBoolean(toBoolNativeData(instance, klass)->value);
    }));
    methods.emplace("equals", gc->create<LoxNativeFunction>(gc, /*arity*/ 1, [klass](auto& instance, auto& args) {
        LOX_ASSERT(args.size() == 1);
        auto other = std::dynamic_pointer_cast<LoxInstance>(args[0]);
        if (!other || other->klass().get() != klass) {
            return toLoxBoolean(false);
        }
        return toLoxBoolean(toBoolNativeData(instance, klass)->value == toBoolNativeData(other, klass)->value);
    }));
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createBoolClass(GarbageCollector* gc)
{
    auto methodFactory = [gc](LoxClass* klass) {
        return createBoolMethods(gc, klass);
    };

    auto dataFactory = [gc]() {
        return gc->create<BoolData>();
    };

    return gc->create<LoxClass>(gc, "Bool", /*superclass*/ nullptr, std::move(methodFactory), std::move(dataFactory));
}

} // namespace cloxx
