#include "LoxNativeFunction.hpp"

#include "Runtime.hpp"

namespace cloxx {

LoxNativeFunction::LoxNativeFunction(PrivateCreationTag tag, Runtime* runtime, size_t arity, Body body,
                                     std::shared_ptr<LoxObject> const& instance)
    : LoxFunction{tag, runtime->functionClass()}, _runtime{runtime}, _arity{arity}, _body{std::move(body)},
      _instance{instance}
{}

std::string LoxNativeFunction::toString()
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
    LoxFunction::enumerateTraceables(enumerator);

    if (_instance) {
        enumerator.enumerate(*this);
    }
}

void LoxNativeFunction::reclaim()
{
    LoxFunction::reclaim();

    _instance.reset();
}

std::shared_ptr<LoxFunction> LoxNativeFunction::bind(std::shared_ptr<LoxObject> const& instance) const
{
    return _runtime->create<LoxNativeFunction>(_runtime, _arity, _body, instance);
}

} // namespace cloxx