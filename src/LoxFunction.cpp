#include "LoxFunction.hpp"

#include "Lox.hpp"

#include "Assert.hpp"
#include "Environment.hpp"
#include "LoxInstance.hpp"

#include "ast/Stmt.hpp"

namespace cloxx {

LoxFunction::LoxFunction(Lox* lox, std::shared_ptr<Environment> const& closure, bool isInitializer, Token const& name,
                         std::vector<Token> const& params, std::vector<std::shared_ptr<Stmt>> const& body,
                         Executor const& executor)
    : _lox{lox}, _closure{closure},
      _isInitializer{isInitializer}, _name{name}, _params{params}, _body{body}, _executor{executor}
{
    LOX_ASSERT(_closure);
}

std::shared_ptr<LoxFunction> LoxFunction::bind(std::shared_ptr<LoxInstance> const& instance) const
{
    LOX_ASSERT(instance);

    auto closure = _lox->create<Environment>(_closure);
    closure->define("this", instance);
    return _lox->create<LoxFunction>(_lox, closure, _isInitializer, _name, _params, _body, _executor);
}

std::string LoxFunction::toString() const
{
    return "<fn " + _name.lexeme + ">";
}

size_t LoxFunction::arity() const
{
    return _params.size();
}

std::shared_ptr<LoxObject> LoxFunction::call(std::vector<std::shared_ptr<LoxObject>> const& args)
{
    LOX_ASSERT(args.size() == _params.size());

    auto env = _lox->create<Environment>(_closure);
    for (size_t i = 0; i < _params.size(); i++) {
        env->define(_params[i].lexeme, args[i]);
    }

    if (_isInitializer) {
        _executor(env, _body);
        return makeLoxNil();
    }

    return _executor(env, _body);
}

void LoxFunction::enumerateTraceables(Traceable::Enumerator const& enumerator)
{
    LOX_ASSERT(_closure);
    enumerator.enumerate(*_closure);
}

void LoxFunction::reclaim()
{
    _closure.reset();
}

} // namespace cloxx
