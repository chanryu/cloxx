#pragma once

#include <memory>
#include <vector>

namespace cloxx {

class LoxObject;

class Callable {
public:
    virtual size_t arity() const = 0;
    virtual std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) = 0;
};

} // namespace cloxx
