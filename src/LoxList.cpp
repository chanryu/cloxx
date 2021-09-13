#include "LoxList.hpp"

#include "Assert.hpp"
#include "LoxClass.hpp"
#include "LoxInstance.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNumber.hpp"

namespace cloxx {

namespace {
class LoxListInstance : public LoxInstance {
public:
    using LoxInstance::LoxInstance;

    std::string toString() const override
    {
        std::string str;
        str.push_back('[');
        for (size_t i = 0; i < items.size(); ++i) {
            str.append(items[i]->toString());
            if (i != items.size() - 1) {
                str.append(", ");
            }
        }
        str.push_back(']');
        return str;
    }

    std::vector<std::shared_ptr<LoxObject>> items;
};

auto& getListItems(std::shared_ptr<LoxInstance> const& instance)
{
    LOX_ASSERT(std::dynamic_pointer_cast<LoxListInstance>(instance));
    return static_cast<LoxListInstance*>(instance.get())->items;
}

} // namespace

std::shared_ptr<LoxClass> createListClass(GarbageCollector* gc)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;

    methods.emplace("append",
                    gc->create<LoxNativeFunction>(gc, /*arity*/ 1, [](auto const& instance, auto const& args) {
                        LOX_ASSERT(args.size() == 1);
                        auto& items = getListItems(instance);
                        items.push_back(args[0]);
                        return args[0];
                    }));

    methods.emplace("length",
                    gc->create<LoxNativeFunction>(gc, /*arity*/ 0, [](auto const& instance, auto const& /*args*/) {
                        auto& items = getListItems(instance);
                        return toLoxNumber(items.size());
                    }));

    return gc->create<LoxNativeClass<LoxListInstance>>(gc, "List", /*superclass*/ nullptr, std::move(methods));
}

} // namespace cloxx
