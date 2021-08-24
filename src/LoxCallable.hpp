#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxCallable : public LoxObject {
public:
    virtual size_t arity() const = 0;
    virtual std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) = 0;
};

} // namespace cloxx
