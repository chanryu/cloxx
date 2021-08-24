#pragma once

#include <map>

#include "GC.hpp"
#include "LoxCallable.hpp"

namespace cloxx {

class LoxFunction;

class LoxClass : public LoxCallable, public Traceable, public std::enable_shared_from_this<LoxClass> {
public:
    LoxClass(PrivateCreationTag tag, GarbageCollector* gc, std::string name,
             std::shared_ptr<LoxClass> const& superclass, std::map<std::string, std::shared_ptr<LoxFunction>> methods);

    std::shared_ptr<LoxFunction> findMethod(std::string const& name) const;

    std::string toString() const override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

private:
    GarbageCollector* const _gc;
    std::string _name;
    std::shared_ptr<LoxClass> _superclass;
    std::map<std::string, std::shared_ptr<LoxFunction>> _methods;
};

} // namespace cloxx
