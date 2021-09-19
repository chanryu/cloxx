#include "LoxString.hpp"

#include "Interpreter.hpp"
#include "LoxClass.hpp"

namespace cloxx {

std::string LoxStringInstance::toString()
{
    return value;
}

bool LoxStringInstance::equals(std::shared_ptr<LoxObject> const& object)
{
    if (LoxInstance::equals(object)) {
        return true;
    }

    if (auto str = dynamic_cast<LoxStringInstance const*>(object.get())) {
        return value == str->value;
    }

    return false;
}

namespace {

class LoxStringClass : public LoxClass {
public:
    using LoxClass::LoxClass;

    std::shared_ptr<LoxInstance> createInstance(std::shared_ptr<LoxClass> const& klass) override
    {
        return _interpreter->create<LoxStringInstance>(klass);
    }
};

std::map<std::string, std::shared_ptr<LoxFunction>> createStringMethods(Interpreter* /*interpreter*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

std::shared_ptr<LoxClass> createStringClass(Interpreter* interpreter)
{
    return interpreter->create<LoxStringClass>(interpreter, "String", /*superclass*/ nullptr,
                                               createStringMethods(interpreter));
}

} // namespace cloxx
