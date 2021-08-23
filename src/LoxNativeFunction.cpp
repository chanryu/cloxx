#include "LoxNativeFunction.hpp"

namespace cloxx {

LoxNativeFunction::LoxNativeFunction(size_t arity, Body body) : _arity{arity}, _body{std::move(body)}
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
    return _body(args);
}

} // namespace cloxx