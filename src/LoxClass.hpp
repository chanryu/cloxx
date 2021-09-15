#pragma once

#include <functional>
#include <map>

#include "Callable.hpp"
#include "GC.hpp"
#include "LoxObject.hpp"

namespace cloxx {

class LoxFunction;
class LoxInstance;

enum class LoxClassId : size_t {}; // FIXME: replace this with pointer value

using LoxMethodFactory = std::function<std::map<std::string, std::shared_ptr<LoxFunction>>(LoxClassId)>;

class LoxClass : public LoxObject, public Callable, public Traceable, public std::enable_shared_from_this<LoxClass> {
public:
    LoxClass(PrivateCreationTag tag, GarbageCollector* gc, std::string name,
             std::shared_ptr<LoxClass> const& superclass, std::map<std::string, std::shared_ptr<LoxFunction>> methods);

    LoxClass(PrivateCreationTag tag, GarbageCollector* gc, std::string name,
             std::shared_ptr<LoxClass> const& superclass, LoxMethodFactory methodFactory);

    LoxClassId classId() const;
    std::shared_ptr<LoxClass> superclass() const;

    std::shared_ptr<LoxFunction> findMethod(std::string const& name) const;

    std::string toString() override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

    std::shared_ptr<Traceable> createNativeData()
    {
        return createNativeData(_gc);
    }

    // FIXME: Make this a dependency functor and remove LoxNativeClass
    // Override to provide native data
    virtual std::shared_ptr<Traceable> createNativeData(GarbageCollector* /*gc*/)
    {
        return nullptr;
    }

private:
    LoxClassId const _classId;

    GarbageCollector* _gc;
    std::string _name;
    std::shared_ptr<LoxClass> _superclass;
    std::map<std::string, std::shared_ptr<LoxFunction>> _methods;
};

template <typename T>
class LoxNativeClass : public LoxClass {
public:
    using LoxClass::LoxClass;

    std::shared_ptr<Traceable> createNativeData(GarbageCollector* gc) override
    {
        return gc->create<T>();
    }
};

} // namespace cloxx
