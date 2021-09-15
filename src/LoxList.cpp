#include "LoxList.hpp"

#include "Assert.hpp"
#include "LoxBoolean.hpp"
#include "LoxClass.hpp"
#include "LoxInstance.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"
#include "LoxNumber.hpp"
#include "LoxString.hpp"

namespace cloxx {

namespace {
struct ListNativeData : Traceable {
    ListNativeData(PrivateCreationTag tag) : Traceable{tag}
    {}

    void enumerateTraceables(Enumerator const& enumerator) override
    {
        for (auto& item : items) {
            if (auto traceable = dynamic_cast<Traceable*>(item.get())) {
                enumerator.enumerate(*traceable);
            }
        }
    }

    void reclaim() override
    {
        items.clear();
    }

    std::vector<std::shared_ptr<LoxObject>> items;
    bool isStringifying = false;
};

auto toListNativeData(std::shared_ptr<LoxInstance> const& instance, LoxClassId classId)
{
    auto nativeData = instance->getNativeData(classId);
    LOX_ASSERT(std::dynamic_pointer_cast<ListNativeData>(nativeData));
    return static_cast<ListNativeData*>(nativeData.get());
}

std::map<std::string, std::shared_ptr<LoxFunction>> createListMethods(GarbageCollector* gc, LoxClassId classId)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;

    methods.emplace("append", gc->create<LoxNativeFunction>(gc, /*arity*/ 1, [classId](auto& instance, auto& args) {
        LOX_ASSERT(args.size() == 1);
        auto& items = toListNativeData(instance, classId)->items;
        items.push_back(args[0]);
        return args[0];
    }));

    methods.emplace("get", gc->create<LoxNativeFunction>(gc, /*arity*/ 1, [classId](auto& instance, auto& args) {
        LOX_ASSERT(args.size() == 1);

        std::shared_ptr<LoxObject> result;
        if (auto number = dynamic_cast<LoxNumber*>(args[0].get())) {
            auto& items = toListNativeData(instance, classId)->items;
            if (auto index = static_cast<size_t>(number->value); index < items.size()) {
                result = items[index];
            }
        }
        if (!result) {
            result = makeLoxNil(); // FIXME: throw exception
        }
        return result;
    }));

    methods.emplace("set", gc->create<LoxNativeFunction>(gc, /*arity*/ 2, [classId](auto& instance, auto& args) {
        LOX_ASSERT(args.size() == 2);

        std::shared_ptr<LoxObject> result;
        if (auto number = dynamic_cast<LoxNumber*>(args[0].get())) {
            auto& items = toListNativeData(instance, classId)->items;
            if (auto index = static_cast<size_t>(number->value); index < items.size()) {
                items[index] = args[1];
                return toLoxBoolean(true);
            }
        }
        return toLoxBoolean(false);
    }));

    methods.emplace("length", gc->create<LoxNativeFunction>(gc, /*arity*/ 0, [classId](auto& instance, auto& /*args*/) {
        auto& items = toListNativeData(instance, classId)->items;
        return toLoxNumber(items.size());
    }));

    methods.emplace("toString",
                    gc->create<LoxNativeFunction>(gc, /*arity*/ 0, [classId](auto& instance, auto& /*args*/) {
                        auto listNativeData = toListNativeData(instance, classId);

                        if (listNativeData->isStringifying) {
                            return toLoxString("[...]");
                        }

                        listNativeData->isStringifying = true;

                        auto const& items = listNativeData->items;
                        std::string str;
                        str.push_back('[');
                        for (size_t i = 0; i < items.size(); ++i) {
                            str.append(items[i]->toString());
                            if (i != items.size() - 1) {
                                str.append(", ");
                            }
                        }
                        str.push_back(']');

                        listNativeData->isStringifying = false;

                        return toLoxString(str);
                    }));
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createListClass(GarbageCollector* gc)
{
    return gc->create<LoxNativeClass<ListNativeData>>(gc, "List", /*superclass*/ nullptr, [gc](LoxClassId classId) {
        return createListMethods(gc, classId);
    });
}

} // namespace cloxx
