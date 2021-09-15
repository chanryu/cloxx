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

using LoxNativeDataFactory = std::function<std::shared_ptr<Traceable>()>;
using LoxNativeMethodFactory = std::function<std::map<std::string, std::shared_ptr<LoxFunction>>(LoxClass*)>;

class LoxClass : public LoxObject, public Callable, public Traceable, public std::enable_shared_from_this<LoxClass> {
public:
    LoxClass(PrivateCreationTag tag, GarbageCollector* gc, std::string name,
             std::shared_ptr<LoxClass> const& superclass, std::map<std::string, std::shared_ptr<LoxFunction>> methods);

    LoxClass(PrivateCreationTag tag, GarbageCollector* gc, std::string name,
             std::shared_ptr<LoxClass> const& superclass, LoxNativeMethodFactory methodFactory,
             LoxNativeDataFactory dataFactory);

    std::shared_ptr<LoxClass> superclass() const;

    std::shared_ptr<LoxFunction> findMethod(std::string const& name) const;

    std::string toString() override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

    std::shared_ptr<Traceable> createInstanceData();

private:
    GarbageCollector* _gc;
    std::string _name;
    std::shared_ptr<LoxClass> _superclass;
    std::map<std::string, std::shared_ptr<LoxFunction>> _methods;
    LoxNativeDataFactory _nativeDataFactory;
};

} // namespace cloxx
