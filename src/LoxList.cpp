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
};

auto& toListItems(std::shared_ptr<Traceable> const& nativeData)
{
    LOX_ASSERT(std::dynamic_pointer_cast<ListNativeData>(nativeData));
    return static_cast<ListNativeData*>(nativeData.get())->items;
}

} // namespace

std::shared_ptr<LoxClass> createListClass(GarbageCollector* gc)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;

    methods.emplace("append", gc->create<LoxNativeFunction>(gc, /*arity*/ 1, [](auto& nativeData, auto& args) {
        LOX_ASSERT(args.size() == 1);
        auto& items = toListItems(nativeData);
        items.push_back(args[0]);
        return args[0];
    }));

    methods.emplace("get", gc->create<LoxNativeFunction>(gc, /*arity*/ 1, [](auto& nativeData, auto& args) {
        LOX_ASSERT(args.size() == 1);

        std::shared_ptr<LoxObject> result;
        if (auto number = dynamic_cast<LoxNumber*>(args[0].get())) {
            auto& items = toListItems(nativeData);
            if (auto index = static_cast<size_t>(number->value); index < items.size()) {
                result = items[index];
            }
        }
        if (!result) {
            result = makeLoxNil(); // FIXME: throw exception
        }
        return result;
    }));

    methods.emplace("set", gc->create<LoxNativeFunction>(gc, /*arity*/ 2, [](auto& nativeData, auto& args) {
        LOX_ASSERT(args.size() == 2);

        std::shared_ptr<LoxObject> result;
        if (auto number = dynamic_cast<LoxNumber*>(args[0].get())) {
            auto& items = toListItems(nativeData);
            if (auto index = static_cast<size_t>(number->value); index < items.size()) {
                items[index] = args[1];
                return toLoxBoolean(true);
            }
        }
        return toLoxBoolean(false);
    }));

    methods.emplace("length", gc->create<LoxNativeFunction>(gc, /*arity*/ 0, [](auto& nativeData, auto& /*args*/) {
        auto& items = toListItems(nativeData);
        return toLoxNumber(items.size());
    }));

    methods.emplace("toString", gc->create<LoxNativeFunction>(gc, /*arity*/ 0, [](auto& nativeData, auto& /*args*/) {
        auto& items = toListItems(nativeData);
        std::string str;
        str.push_back('[');
        for (size_t i = 0; i < items.size(); ++i) {
            str.append(items[i]->toString());
            if (i != items.size() - 1) {
                str.append(", ");
            }
        }
        str.push_back(']');
        return toLoxString(str);
    }));

    return gc->create<LoxNativeClass<ListNativeData>>(gc, "List", /*superclass*/ nullptr, std::move(methods));
}

} // namespace cloxx
