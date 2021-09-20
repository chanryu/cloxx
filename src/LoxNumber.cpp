#include "LoxNumber.hpp"

#include "Interpreter.hpp"
#include "LoxObject.hpp"

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

std::map<std::string, std::shared_ptr<LoxFunction>> createNumberMethods(Interpreter* /*interpreter*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

class LoxNumberClass : public LoxObjectClass {
public:
    LoxNumberClass(PrivateCreationTag tag, Interpreter* interpreter, std::string name,
                   std::shared_ptr<LoxObjectClass> const& superclass)
        : LoxObjectClass{tag, interpreter, std::move(name), superclass, createNumberMethods(interpreter)}
    {}

    std::shared_ptr<LoxObject> createInstance(std::shared_ptr<LoxObjectClass> const& klass) override
    {
        return _interpreter->create<LoxNumber>(klass);
    }
};

} // namespace

std::shared_ptr<LoxObjectClass> createNumberClass(Interpreter* interpreter)
{
    return interpreter->create<LoxNumberClass>(interpreter, "Number", /*superclass*/ nullptr);
}

} // namespace cloxx
