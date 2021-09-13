#pragma once

#include <functional>

#include "LoxFunction.hpp"

namespace cloxx {

class LoxNativeFunction : public LoxFunction {
public:
    using Body = std::function<std::shared_ptr<LoxObject>(std::shared_ptr<LoxInstance> const& instance,
                                                          std::vector<std::shared_ptr<LoxObject>> const&)>;

    LoxNativeFunction(PrivateCreationTag tag, GarbageCollector* gc, size_t arity, Body body,
                      std::shared_ptr<LoxInstance> const& instance = nullptr);

    std::string toString() const override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> const& instance) const override;

private:
    GarbageCollector* const _gc;
    size_t const _arity;
    Body const _body;

    std::shared_ptr<LoxInstance> _instance;
};

} // namespace cloxx
