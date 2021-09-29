#pragma once

#include <functional>

#include "LoxFunction.hpp"

namespace cloxx {

class Runtime;
class Traceable;

class LoxNativeFunction : public LoxFunction {
public:
    using Body = std::function<std::shared_ptr<LoxObject>(std::shared_ptr<LoxObject> const& instance,
                                                          std::vector<std::shared_ptr<LoxObject>> const&)>;

    LoxNativeFunction(PrivateCreationTag tag, Runtime* runtime, size_t arity, Body body,
                      std::shared_ptr<LoxObject> const& instance = nullptr);

    std::string toString() override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxObject> const& instance) const override;

private:
    Runtime* const _runtime;
    size_t const _arity;
    Body const _body;

    std::shared_ptr<LoxObject> _instance;
};

} // namespace cloxx
