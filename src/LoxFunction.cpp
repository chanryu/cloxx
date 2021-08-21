#include "LoxFunction.hpp"

#include "Assert.hpp"
#include "Environment.hpp"
#include "LoxInstance.hpp"

#include "ast/Stmt.hpp"

namespace cloxx {

LoxFunction::LoxFunction(bool isInitializer, std::shared_ptr<Environment> const& closure, Token const& name,
                         std::vector<Token> const& params, Body body)
    : _isInitializer{isInitializer}, _closure{closure}, _name{name}, _params{params}, _body{std::move(body)}
{}

std::shared_ptr<LoxFunction> LoxFunction::bind(std::shared_ptr<LoxInstance> const& instance) const
{
    LOX_ASSERT(instance);

    auto closure = std::make_shared<Environment>(_closure);
    closure->define("this", instance);
    return std::make_shared<LoxFunction>(_isInitializer, closure, _name, _params, _body);
}

std::string LoxFunction::toString() const
{
    return "<function " + _name.lexeme + ">";
}

size_t LoxFunction::arity() const
{
    return _params.size();
}

std::shared_ptr<LoxObject> LoxFunction::call(std::vector<std::shared_ptr<LoxObject>> const& args)
{
    LOX_ASSERT(args.size() == _params.size());

    auto env = std::make_shared<Environment>(_closure);
    for (size_t i = 0; i < _params.size(); i++) {
        env->define(_params[i].lexeme, args[i]);
    }

    if (_isInitializer) {
        _body(env);
        return makeLoxNil();
    }
    else {
        return _body(env);
    }
}

////////////

LoxNativeFunction::LoxNativeFunction(std::string name, size_t arity, Body body)
    : _name{std::move(name)}, _arity{arity}, _body{std::move(body)}
{}

std::string LoxNativeFunction::toString() const
{
    return "<fn native:" + _name + ">";
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
