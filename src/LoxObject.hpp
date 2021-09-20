#pragma once

#include <map>
#include <memory>
#include <string>

#include "Callable.hpp"
#include "GC.hpp"

namespace cloxx {

class LoxObjectClass;
class LoxFunction;
class Interpreter;

struct Token;

class LoxObject : public Traceable, public std::enable_shared_from_this<LoxObject> {
public:
    LoxObject(PrivateCreationTag tag, std::shared_ptr<LoxObjectClass> const& klass);
    LoxObject(PrivateCreationTag tag, std::map<std::string, std::shared_ptr<LoxObject>> fields);
    virtual ~LoxObject();

#ifdef CLOXX_GC_DEBUG
    static size_t instanceCount();
#endif

    std::shared_ptr<LoxObject> get(Token const& name);
    void set(Token const& name, std::shared_ptr<LoxObject> const& value);

    virtual std::string toString();
    virtual bool isTruthy();
    virtual bool equals(std::shared_ptr<LoxObject> const& object);

    // GC support
    void enumerateTraceables(Enumerator const& enumerator) override;
    void reclaim() override;

private:
    std::shared_ptr<LoxObjectClass> _class;
    std::map<std::string, std::shared_ptr<LoxObject>> _fields;
};

class LoxObjectClass : public LoxObject, public Callable {
public:
    LoxObjectClass(PrivateCreationTag tag, Interpreter* interpreter, std::string name,
                   std::shared_ptr<LoxObjectClass> const& superclass,
                   std::map<std::string, std::shared_ptr<LoxFunction>> methods);

    std::shared_ptr<LoxFunction> findMethod(std::string const& name) const;

    std::string toString() override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

    virtual std::shared_ptr<LoxObject> createInstance(std::shared_ptr<LoxObjectClass> const& klass);

protected:
    Interpreter* const _interpreter;

private:
    std::string _name;
    std::shared_ptr<LoxObjectClass> _superclass;
    std::map<std::string, std::shared_ptr<LoxFunction>> _methods;
};

} // namespace cloxx
