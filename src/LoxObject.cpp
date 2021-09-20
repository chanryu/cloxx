#include "LoxObject.hpp"

#include "Assert.hpp"
#include "Interpreter.hpp"
#include "RuntimeError.hpp"
#include "Token.hpp"

#include "LoxFunction.hpp"

namespace cloxx {

#ifdef CLOXX_GC_DEBUG
namespace {
size_t objectInstanceCount = 0;
}
#endif

LoxObject::LoxObject(PrivateCreationTag tag, std::shared_ptr<LoxObjectClass> const& klass)
    : Traceable{tag}, _class{klass}
{
    LOX_ASSERT(_class);
#ifdef CLOXX_GC_DEBUG
    objectInstanceCount += 1;
#endif
}

LoxObject::LoxObject(PrivateCreationTag tag, std::map<std::string, std::shared_ptr<LoxObject>> fields)
    : Traceable{tag}, _fields{std::move(fields)}
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

///////

namespace {
auto createClassFields(LoxObjectClass* /*klass*/)
{
    std::map<std::string, std::shared_ptr<LoxObject>> fields;
    // TODO: new()
    return fields;
}
} // namespace

LoxObjectClass::LoxObjectClass(PrivateCreationTag tag, Interpreter* interpreter, std::string name,
                               std::shared_ptr<LoxObjectClass> const& superclass,
                               std::map<std::string, std::shared_ptr<LoxFunction>> methods)
    : LoxObject{tag, createClassFields(this)}, _interpreter{interpreter}, _name{std::move(name)},
      _superclass{superclass}, _methods{std::move(methods)}
{}

std::shared_ptr<LoxFunction> LoxObjectClass::findMethod(std::string const& name) const
{
    if (auto it = _methods.find(name); it != _methods.end()) {
        return it->second;
    }

    if (_superclass) {
        return _superclass->findMethod(name);
    }

    return nullptr;
}

std::string LoxObjectClass::toString()
{
    return _name;
}

size_t LoxObjectClass::arity() const
{
    if (auto initializer = findMethod("init")) {
        return initializer->arity();
    }

    return 0;
}

std::shared_ptr<LoxObject> LoxObjectClass::call(std::vector<std::shared_ptr<LoxObject>> const& args)
{
    auto instance = createInstance(std::static_pointer_cast<LoxObjectClass>(shared_from_this()));

    if (auto initializer = findMethod("init")) {
        initializer->bind(instance)->call(args);
    }

    return instance;
}

void LoxObjectClass::enumerateTraceables(Traceable::Enumerator const& enumerator)
{
    if (_superclass) {
        enumerator.enumerate(*_superclass);
    }

    for (auto& [_, method] : _methods) {
        enumerator.enumerate(*method);
    }
}

void LoxObjectClass::reclaim()
{
    _superclass.reset();
    _methods.clear();
}

std::shared_ptr<LoxObject> LoxObjectClass::createInstance(std::shared_ptr<LoxObjectClass> const& klass)
{
    if (_superclass) {
        return _superclass->createInstance(klass);
    }

    return _interpreter->create<LoxObject>(klass);
}

} // namespace cloxx
