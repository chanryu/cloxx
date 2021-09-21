#include "LoxUserFunction.hpp"

#include "Assert.hpp"
#include "Environment.hpp"
#include "Runtime.hpp"

#include "LoxObject.hpp"

#include "ast/Stmt.hpp"

namespace cloxx {

LoxUserFunction::LoxUserFunction(PrivateCreationTag tag, Runtime* runtime, std::shared_ptr<Environment> const& closure,
                                 bool isInitializer, Token const& name, std::vector<Token> const& params,
                                 std::vector<Stmt> const& body, Executor const& executor)
    : LoxFunction{tag, runtime->functionClass()}, _runtime{runtime}, _closure{closure},
      _isInitializer{isInitializer}, _name{name}, _params{params}, _body{body}, _executor{executor}
{
    LOX_ASSERT(_closure);
}

std::shared_ptr<LoxFunction> LoxUserFunction::bind(std::shared_ptr<LoxObject> const& instance) const
{
    LOX_ASSERT(instance);

    auto closure = _runtime->create<Environment>(_closure);
    closure->define("this", instance);
    return _runtime->create<LoxUserFunction>(_runtime, closure, _isInitializer, _name, _params, _body, _executor);
}

std::string LoxUserFunction::toString()
{
    return "<fn " + _name.lexeme + ">";
}

size_t LoxUserFunction::arity() const
{
    return _params.size();
}

std::shared_ptr<LoxObject> LoxUserFunction::call(std::vector<std::shared_ptr<LoxObject>> const& args)
{
    LOX_ASSERT(args.size() == _params.size());

    auto env = _runtime->create<Environment>(_closure);
    for (size_t i = 0; i < _params.size(); i++) {
        env->define(_params[i].lexeme, args[i]);
    }

    if (_isInitializer) {
        _executor(env, _body);
        return _closure->getAt(0, "this");
    }

    return _executor(env, _body);
}

void LoxUserFunction::enumerateTraceables(Traceable::Enumerator const& enumerator)
{
    LoxFunction::enumerateTraceables(enumerator);

    LOX_ASSERT(_closure);
    enumerator.enumerate(*_closure);
}

void LoxUserFunction::reclaim()
{
    LoxFunction::reclaim();
    _closure.reset();
}

} // namespace cloxx
