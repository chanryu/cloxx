#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Callable.hpp"
#include "GC.hpp"
#include "LoxObject.hpp"
#include "Token.hpp"

namespace cloxx {

class Stmt;
class Environment;
class LoxInstance;

class LoxFunction : public LoxObject, public Callable, public Traceable {
public:
    using Executor =
        std::function<std::shared_ptr<LoxObject>(std::shared_ptr<Environment> const&, std::vector<Stmt> const&)>;

    LoxFunction(PrivateCreationTag tag, GarbageCollector* gc, std::shared_ptr<Environment> const& closure,
                bool isInitializer, Token const& name, std::vector<Token> const& params, std::vector<Stmt> const& body,
                Executor const& executor);

    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> const& instance) const;

    std::string toString() const override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

private:
    GarbageCollector* const _gc;

    std::shared_ptr<Environment> _closure;

    bool const _isInitializer;
    Token const _name;
    std::vector<Token> const _params;
    std::vector<Stmt> const _body;
    Executor const _executor;
};

} // namespace cloxx
