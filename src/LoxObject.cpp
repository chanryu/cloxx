#include "LoxObject.hpp"

#include "Assert.hpp"
#include "Interpreter.hpp"
#include "RuntimeError.hpp"
#include "Token.hpp"

#include "LoxClass.hpp"
#include "LoxFunction.hpp"

namespace cloxx {

#ifdef CLOXX_GC_DEBUG
namespace {
size_t objectInstanceCount = 0;
}
#endif

LoxObject::LoxObject(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass) : Traceable{tag}, _class{klass}
{
    LOX_ASSERT(_class);
#ifdef CLOXX_GC_DEBUG
    objectInstanceCount += 1;
#endif
}

LoxObject::LoxObject(PrivateCreationTag tag) : Traceable{tag}
{
#ifdef CLOXX_GC_DEBUG
    objectInstanceCount += 1;
#endif
}

LoxObject::~LoxObject()
{
#ifdef CLOXX_GC_DEBUG
    objectInstanceCount -= 1;
#endif
}

#ifdef CLOXX_GC_DEBUG
size_t LoxObject::instanceCount()
{
    return objectInstanceCount;
}
#endif

std::shared_ptr<LoxObject> LoxObject::get(Token const& name)
{
    if (auto it = _fields.find(name.lexeme); it != _fields.end()) {
        return it->second;
    }

    if (_class) {
        if (auto method = _class->findMethod(name.lexeme)) {
            return method->bind(shared_from_this());
        }
    }
    else {
        // methods Class class, e.g., new()
    }

    throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void LoxObject::set(Token const& name, std::shared_ptr<LoxObject> const& value)
{
    _fields[name.lexeme] = value;
}

std::string LoxObject::toString()
{
    if (auto method = _class->findMethod("toString"); method && method->arity() == 0) {
        return method->bind(shared_from_this())->call({})->toString();
    }

    if (!_class) {
        return "Class";
    }

    return _class->toString() + " instance";
}

bool LoxObject::isTruthy()
{
    return true;
}

bool LoxObject::equals(std::shared_ptr<LoxObject> const& object)
{
    if (_class) {
        if (auto method = _class->findMethod("equals"); method && method->arity() == 1) {
            return method->bind(shared_from_this())->call({object})->isTruthy();
        }
    }

    return this == object.get();
}

void LoxObject::enumerateTraceables(Enumerator const& enumerator)
{
    if (_class) {
        enumerator.enumerate(*_class);
    }

    for (auto& [_, field] : _fields) {
        enumerator.enumerate(*field);
    }
}

void LoxObject::reclaim()
{
    _class.reset();
    _fields.clear();
}

namespace {
std::map<std::string, std::shared_ptr<LoxFunction>> createObjectMethods(Interpreter* /*interpreter*/)
{
    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    // no methods yet
    return methods;
}
} // namespace

std::shared_ptr<LoxClass> createObjectClass(Interpreter* interpreter)
{
    return interpreter->create<LoxClass>(interpreter, "Object", /*superclass*/ nullptr,
                                         createObjectMethods(interpreter));
}

} // namespace cloxx
