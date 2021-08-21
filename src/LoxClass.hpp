#pragma once

#include <map>

#include "LoxObject.hpp"

namespace cloxx {

class LoxFunction;

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass> {
public:
    LoxClass(std::string name, std::shared_ptr<LoxClass> const& superclass,
             std::map<std::string, std::shared_ptr<LoxFunction>> methods);

    std::shared_ptr<LoxFunction> findMethod(std::string const& name) const;

    std::string toString() const override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    std::string const name;

private:
    std::shared_ptr<LoxClass> const _superclass;
    std::map<std::string, std::shared_ptr<LoxFunction>> const _methods;
};

} // namespace cloxx
