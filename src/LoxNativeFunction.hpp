#pragma once

#include <functional>

#include "Callable.hpp"
#include "LoxObject.hpp"

namespace cloxx {

class LoxNativeFunction : public LoxObject, public Callable {
public:
    using Body = std::function<std::shared_ptr<LoxObject>(std::vector<std::shared_ptr<LoxObject>> const&)>;

    LoxNativeFunction(size_t arity, Body body);

    std::string toString() const override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

private:
    size_t const _arity;
    Body const _body;
};

} // namespace cloxx
