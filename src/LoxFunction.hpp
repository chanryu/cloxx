#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "LoxObject.hpp"
#include "Token.hpp"

namespace cloxx {

class Stmt;
class Environment;
class LoxInstance;

class LoxFunction : public LoxCallable {
public:
    using Body = std::function<std::shared_ptr<LoxObject>(std::shared_ptr<Environment> const&)>;

    LoxFunction(bool isInitializer, std::shared_ptr<Environment> const& closure, Token const& name,
                std::vector<Token> const& params, Body body);

    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> const& instance) const;

    std::string toString() const override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

private:
    bool const _isInitializer;
    std::shared_ptr<Environment> const _closure;

    Token const _name;
    std::vector<Token> const _params;
    Body const _body;
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
