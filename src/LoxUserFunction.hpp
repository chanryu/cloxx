#pragma once

#include <functional>
#include <string>

#include "LoxFunction.hpp"
#include "Token.hpp"

namespace cloxx {

class Stmt;
class Environment;
class Interpreter;

class LoxUserFunction : public LoxFunction {
public:
    using Body = std::vector<Stmt>;
    using Executor = std::function<std::shared_ptr<LoxObject>(std::shared_ptr<Environment> const&, Body const&)>;

    LoxUserFunction(PrivateCreationTag tag, Interpreter* interpreter, std::shared_ptr<Environment> const& closure,
                    bool isInitializer, Token const& name, std::vector<Token> const& params,
                    std::vector<Stmt> const& body, Executor const& executor);

    std::string toString() override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxObject> const& instance) const override;

private:
    Interpreter* const _interpreter;

    std::shared_ptr<Environment> _closure;

    bool const _isInitializer;
    Token const _name;
    std::vector<Token> const _params;
    std::vector<Stmt> const _body;
    Executor const _executor;
};

} // namespace cloxx
