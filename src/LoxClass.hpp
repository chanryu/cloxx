#pragma once

#include <functional>
#include <map>

#include "Callable.hpp"
#include "GC.hpp"
#include "LoxObject.hpp"

namespace cloxx {

class LoxClass;
class LoxFunction;
class LoxInstance;

class Interpreter;

class LoxClass : public LoxObject, public Callable, public std::enable_shared_from_this<LoxClass> {
public:
    LoxClass(PrivateCreationTag tag, Interpreter* interpreter, std::string name,
             std::shared_ptr<LoxClass> const& superclass, std::map<std::string, std::shared_ptr<LoxFunction>> methods);

    std::shared_ptr<LoxFunction> findMethod(std::string const& name) const;

    std::string toString() override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

    virtual std::shared_ptr<LoxInstance> createInstance(std::shared_ptr<LoxClass> const& klass);

protected:
    Interpreter* const _interpreter;

private:
    std::string _name;
    std::shared_ptr<LoxClass> _superclass;
    std::map<std::string, std::shared_ptr<LoxFunction>> _methods;
};

} // namespace cloxx
