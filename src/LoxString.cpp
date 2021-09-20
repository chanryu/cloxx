#include "LoxString.hpp"

#include "Interpreter.hpp"
#include "LoxObject.hpp"

namespace cloxx {

std::string LoxString::toString()
{
    return value;
}

bool LoxString::equals(std::shared_ptr<LoxObject> const& object)
{
    if (LoxObject::equals(object)) {
        return true;
    }

    if (auto str = dynamic_cast<LoxString const*>(object.get())) {
        return value == str->value;
    }

    return false;
}

namespace {

class LoxStringClass : public LoxObjectClass {
public:
    using LoxObjectClass::LoxObjectClass;

    std::shared_ptr<LoxObject> createInstance(std::shared_ptr<LoxObjectClass> const& klass) override
    {
        return _interpreter->create<LoxString>(klass);
    }
};

std::map<std::string, std::shared_ptr<LoxFunction>> createStringMethods(Interpreter* /*interpreter*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}

} // namespace

std::shared_ptr<LoxObjectClass> createStringClass(Interpreter* interpreter)
{
    return interpreter->create<LoxStringClass>(interpreter, "String", /*superclass*/ nullptr,
                                               createStringMethods(interpreter));
}

} // namespace cloxx
