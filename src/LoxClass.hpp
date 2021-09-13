#pragma once

#include <map>

#include "Callable.hpp"
#include "GC.hpp"
#include "LoxObject.hpp"

namespace cloxx {

class LoxFunction;
class LoxInstance;

class LoxClass : public LoxObject, public Callable, public Traceable, public std::enable_shared_from_this<LoxClass> {
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

    virtual std::shared_ptr<LoxInstance> createInstance(GarbageCollector* gc) = 0;

private:
    GarbageCollector* _gc;
    std::string _name;
    std::shared_ptr<LoxClass> _superclass;
    std::map<std::string, std::shared_ptr<LoxFunction>> _methods;
};

class LoxUserClass : public LoxClass {
public:
    using LoxClass::LoxClass;

    std::shared_ptr<LoxInstance> createInstance(GarbageCollector* gc) override;
};

template <typename T>
class LoxNativeClass : public LoxClass {
public:
    using LoxClass::LoxClass;

    std::shared_ptr<LoxInstance> createInstance(GarbageCollector* gc) override
    {
        return gc->create<T>(shared_from_this());
    }
};

} // namespace cloxx
