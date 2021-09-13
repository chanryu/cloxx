#include "LoxNativeFunction.hpp"

#include "LoxInstance.hpp"

namespace cloxx {

LoxNativeFunction::LoxNativeFunction(PrivateCreationTag tag, GarbageCollector* gc, size_t arity, Body body,
                                     std::shared_ptr<LoxInstance> const& instance)
    : LoxFunction{tag}, _gc{gc}, _arity{arity}, _body{std::move(body)}, _instance{instance}
{}

std::string LoxNativeFunction::toString() const
{
    return "<native fn>";
}

size_t LoxNativeFunction::arity() const
{
    return _arity;
}

std::shared_ptr<LoxObject> LoxNativeFunction::call(std::vector<std::shared_ptr<LoxObject>> const& args)
{
    return _body(_instance, args);
}

void LoxNativeFunction::enumerateTraceables(Traceable::Enumerator const& enumerator)
{
    if (_instance) {
        enumerator.enumerate(*this);
    }
}

void LoxNativeFunction::reclaim()
{
    _instance.reset();
}

std::shared_ptr<LoxFunction> LoxNativeFunction::bind(std::shared_ptr<LoxInstance> const& instance) const
{
    return _gc->create<LoxNativeFunction>(_gc, _arity, _body, instance);
}

} // namespace cloxx