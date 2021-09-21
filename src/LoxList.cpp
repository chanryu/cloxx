#include "LoxList.hpp"

#include "Assert.hpp"
#include "Runtime.hpp"

#include "LoxClass.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"
#include "LoxNumber.hpp"
#include "LoxString.hpp"

namespace cloxx {

namespace {
class LoxList : public LoxObject {
public:
    using LoxObject::LoxObject;

    void enumerateTraceables(Enumerator const& enumerator) override
    {
        LoxObject::enumerateTraceables(enumerator);

        for (auto& item : items) {
            if (auto traceable = dynamic_cast<Traceable*>(item.get())) {
                enumerator.enumerate(*traceable);
            }
        }
    }

    void reclaim() override
    {
        LoxObject::reclaim();

        items.clear();
    }

    std::vector<std::shared_ptr<LoxObject>> items;
    bool isStringifying = false;
};

class LoxListClass : public LoxClass {
public:
    using LoxClass::LoxClass;

    std::shared_ptr<LoxObject> createInstance(std::shared_ptr<LoxClass> const& klass) override
    {
        return _runtime->create<LoxList>(klass);
    }
};

auto toListInstance(std::shared_ptr<LoxObject> const& instance)
{
    LOX_ASSERT(std::dynamic_pointer_cast<LoxList>(instance));
    return static_cast<LoxList*>(instance.get());
}

std::map<std::string, std::shared_ptr<LoxFunction>> createListMethods(Runtime* runtime)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;

    methods.emplace("append", runtime->create<LoxNativeFunction>(runtime, /*arity*/ 1, [](auto& instance, auto& args) {
        LOX_ASSERT(args.size() == 1);

        auto& items = toListInstance(instance)->items;
        items.push_back(args[0]);
        return args[0];
    }));

    methods.emplace("get",
                    runtime->create<LoxNativeFunction>(runtime, /*arity*/ 1, [runtime](auto& instance, auto& args) {
                        LOX_ASSERT(args.size() == 1);

                        std::shared_ptr<LoxObject> result;
                        if (auto number = dynamic_cast<LoxNumber*>(args[0].get())) {
                            auto& items = toListInstance(instance)->items;
                            if (auto index = static_cast<size_t>(number->value); index < items.size()) {
                                result = items[index];
                            }
                        }
                        if (!result) {
                            result = runtime->getNil(); // FIXME: throw exception
                        }
                        return result;
                    }));

    methods.emplace("set",
                    runtime->create<LoxNativeFunction>(runtime, /*arity*/ 2, [runtime](auto& instance, auto& args) {
                        LOX_ASSERT(args.size() == 2);

                        std::shared_ptr<LoxObject> result;
                        if (auto number = dynamic_cast<LoxNumber*>(args[0].get())) {
                            auto& items = toListInstance(instance)->items;
                            if (auto index = static_cast<size_t>(number->value); index < items.size()) {
                                items[index] = args[1];
                                return runtime->toLoxBool(true);
                            }
                        }
                        return runtime->toLoxBool(false);
                    }));

    methods.emplace("length",
                    runtime->create<LoxNativeFunction>(runtime, /*arity*/ 0, [runtime](auto& instance, auto& /*args*/) {
                        auto& items = toListInstance(instance)->items;
                        return runtime->toLoxNumber(items.size());
                    }));

    methods.emplace("toString",
                    runtime->create<LoxNativeFunction>(runtime, /*arity*/ 0, [runtime](auto& instance, auto& /*args*/) {
                        auto listInstance = toListInstance(instance);

                        if (listInstance->isStringifying) {
                            return runtime->toLoxString("[...]");
                        }

                        listInstance->isStringifying = true;

                        auto const& items = listInstance->items;
                        std::string str;
                        str.push_back('[');
                        for (size_t i = 0; i < items.size(); ++i) {
                            str.append(items[i]->toString());
                            if (i != items.size() - 1) {
                                str.append(", ");
                            }
                        }
                        str.push_back(']');

                        listInstance->isStringifying = false;

                        return runtime->toLoxString(str);
                    }));
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createListClass(Runtime* runtime)
{
    return runtime->create<LoxListClass>(runtime, "List", runtime->objectClass(), createListMethods(runtime));
}

} // namespace cloxx
