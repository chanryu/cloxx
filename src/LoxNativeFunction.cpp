#include "LoxNativeFunction.hpp"

#include "Interpreter.hpp"

namespace cloxx {

LoxNativeFunction::LoxNativeFunction(PrivateCreationTag tag, Interpreter* interpreter, size_t arity, Body body,
                                     std::shared_ptr<LoxInstance> const& instance)
    : LoxFunction{tag}, _interpreter{interpreter}, _arity{arity}, _body{std::move(body)}, _instance{instance}
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
    return _interpreter->create<LoxNativeFunction>(_interpreter, _arity, _body, instance);
}

} // namespace cloxx