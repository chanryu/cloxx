#pragma once

#include <functional>
#include <memory>
#include <string>

#include "LoxObject.hpp"

namespace cloxx {

class LoxFunction;
class Runtime;

using LoxObjectFactory = std::function<std::shared_ptr<LoxObject>(std::shared_ptr<LoxClass> const&)>;

class LoxClass : public LoxObject, public Callable {
public:
    LoxClass(PrivateCreationTag tag, Runtime* runtime, std::string name, std::shared_ptr<LoxClass> const& superclass,
             std::map<std::string, std::shared_ptr<LoxFunction>> methods, LoxObjectFactory objectFactory);

    std::shared_ptr<LoxFunction> findMethod(std::string const& name) const;

    std::string toString() override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

private:
    std::shared_ptr<LoxObject> createInstance(std::shared_ptr<LoxClass> const& klass);

    Runtime* const _runtime;
    std::string _name;
    std::shared_ptr<LoxClass> _superclass;
    std::map<std::string, std::shared_ptr<LoxFunction>> _methods;
    LoxObjectFactory _objectFactory;
};

} // namespace cloxx
