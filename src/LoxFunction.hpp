#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "LoxObject.hpp"
#include "Token.hpp"

namespace cloxx {

class Lox;

class Stmt;
class Environment;
class LoxInstance;

class LoxFunction : public LoxCallable {
public:
    using Executor = std::function<std::shared_ptr<LoxObject>(std::shared_ptr<Environment> const&,
                                                              std::vector<std::shared_ptr<Stmt>> const&)>;

    LoxFunction(Lox* lox, std::shared_ptr<Environment> const& closure, bool isInitializer, Token const& name,
                std::vector<Token> const& params, std::vector<std::shared_ptr<Stmt>> const& body,
                Executor const& executor);

    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> const& instance) const;

    std::string toString() const override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void mark() override;
    void reclaim() override;

private:
    Lox* const _lox; // HACK

    std::shared_ptr<Environment> _closure;

    bool const _isInitializer;
    Token const _name;
    std::vector<Token> const _params;
    std::vector<std::shared_ptr<Stmt>> const _body;
    Executor const _executor;
};

class LoxNativeFunction : public LoxCallable {
public:
    using Body = std::function<std::shared_ptr<LoxObject>(std::vector<std::shared_ptr<LoxObject>> const&)>;

    LoxNativeFunction(std::string name, size_t arity, Body body);

    std::string toString() const override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

private:
    std::string const _name;
    size_t const _arity;
    Body const _body;
};

} // namespace cloxx
