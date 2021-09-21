#include "LoxNumber.hpp"

#include "Interpreter.hpp"
#include "LoxClass.hpp"

namespace cloxx {

std::string LoxNumber::toString()
{
    auto str = std::to_string(value);

    // Remove trailing zeros.
    while (str.size() > 1) {
        char c = str.back();
        if (c == '0') {
            str.pop_back();
        }
        else {
            if (c == '.')
                str.pop_back();
            break;
        }
    }
    return str;
}

bool LoxNumber::equals(std::shared_ptr<LoxObject> const& object)
{
    if (LoxObject::equals(object)) {
        return true;
    }

    if (auto num = dynamic_cast<LoxNumber const*>(object.get())) {
        return value == num->value;
    }

    return false;
}

namespace {

std::map<std::string, std::shared_ptr<LoxFunction>> createNumberMethods(Runtime* /*runtime*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

class LoxNumberClass : public LoxClass {
public:
    LoxNumberClass(PrivateCreationTag tag, Runtime* runtime, std::string name,
                   std::shared_ptr<LoxClass> const& superclass)
        : LoxClass{tag, runtime, std::move(name), superclass, createNumberMethods(runtime)}
    {}

    std::shared_ptr<LoxObject> createInstance(std::shared_ptr<LoxClass> const& klass) override
    {
        return _runtime->create<LoxNumber>(klass);
    }
};

} // namespace

std::shared_ptr<LoxClass> createNumberClass(Runtime* runtime)
{
    return runtime->create<LoxNumberClass>(runtime, "Number", runtime->objectClass());
}

} // namespace cloxx
