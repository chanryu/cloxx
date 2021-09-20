#include "LoxList.hpp"

#include "Assert.hpp"
#include "Interpreter.hpp"
#include "LoxNativeFunction.hpp"
#include "LoxNil.hpp"
#include "LoxNumber.hpp"
#include "LoxObject.hpp"
#include "LoxString.hpp"

namespace cloxx {

namespace {
class LoxListInstance : public LoxObject {
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

class LoxListClass : public LoxObjectClass {
public:
    using LoxObjectClass::LoxObjectClass;

    std::shared_ptr<LoxObject> createInstance(std::shared_ptr<LoxObjectClass> const& klass) override
    {
        return _interpreter->create<LoxListInstance>(klass);
    }
};

auto toListInstance(std::shared_ptr<LoxObject> const& instance)
{
    LOX_ASSERT(std::dynamic_pointer_cast<LoxListInstance>(instance));
    return static_cast<LoxListInstance*>(instance.get());
}

std::map<std::string, std::shared_ptr<LoxFunction>> createListMethods(Interpreter* interpreter)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;

    methods.emplace("append",
                    interpreter->create<LoxNativeFunction>(interpreter, /*arity*/ 1, [](auto& instance, auto& args) {
                        LOX_ASSERT(args.size() == 1);

                        auto& items = toListInstance(instance)->items;
                        items.push_back(args[0]);
                        return args[0];
                    }));

    methods.emplace("get", interpreter->create<LoxNativeFunction>(
                               interpreter, /*arity*/ 1, [interpreter](auto& instance, auto& args) {
                                   LOX_ASSERT(args.size() == 1);

                                   std::shared_ptr<LoxObject> result;
                                   if (auto number = dynamic_cast<LoxNumber*>(args[0].get())) {
                                       auto& items = toListInstance(instance)->items;
                                       if (auto index = static_cast<size_t>(number->value); index < items.size()) {
                                           result = items[index];
                                       }
                                   }
                                   if (!result) {
                                       result = interpreter->makeLoxNil(); // FIXME: throw exception
                                   }
                                   return result;
                               }));

    methods.emplace("set", interpreter->create<LoxNativeFunction>(
                               interpreter, /*arity*/ 2, [interpreter](auto& instance, auto& args) {
                                   LOX_ASSERT(args.size() == 2);

                                   std::shared_ptr<LoxObject> result;
                                   if (auto number = dynamic_cast<LoxNumber*>(args[0].get())) {
                                       auto& items = toListInstance(instance)->items;
                                       if (auto index = static_cast<size_t>(number->value); index < items.size()) {
                                           items[index] = args[1];
                                           return interpreter->toLoxBool(true);
                                       }
                                   }
                                   return interpreter->toLoxBool(false);
                               }));

    methods.emplace("length", interpreter->create<LoxNativeFunction>(interpreter, /*arity*/ 0,
                                                                     [interpreter](auto& instance, auto& /*args*/) {
                                                                         auto& items = toListInstance(instance)->items;
                                                                         return interpreter->toLoxNumber(items.size());
                                                                     }));

    methods.emplace("toString", interpreter->create<LoxNativeFunction>(interpreter, /*arity*/ 0,
                                                                       [interpreter](auto& instance, auto& /*args*/) {
                                                                           auto listInstance = toListInstance(instance);

                                                                           if (listInstance->isStringifying) {
                                                                               return interpreter->toLoxString("[...]");
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

                                                                           return interpreter->toLoxString(str);
                                                                       }));
    return methods;
}

} // namespace

std::shared_ptr<LoxObjectClass> createListClass(Interpreter* interpreter)
{
    return interpreter->create<LoxListClass>(interpreter, "List", /*superclass*/ nullptr,
                                             createListMethods(interpreter));
}

} // namespace cloxx
