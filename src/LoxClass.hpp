#pragma once

#include <map>

#include "GC.hpp"
#include "LoxObject.hpp"

namespace cloxx {

class Lox;
class LoxFunction;

class LoxClass : public LoxCallable, public Traceable, public std::enable_shared_from_this<LoxClass> {
public:
    LoxClass(Lox* lox, std::string name, std::shared_ptr<LoxClass> const& superclass,
             std::map<std::string, std::shared_ptr<LoxFunction>> methods);

    std::shared_ptr<LoxFunction> findMethod(std::string const& name) const;

    std::string toString() const override;

    size_t arity() const override;
    std::shared_ptr<LoxObject> call(std::vector<std::shared_ptr<LoxObject>> const& args) override;

    // GC support
    void enumTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

private:
    Lox* const _lox;
    std::string _name;
    std::shared_ptr<LoxClass> _superclass;
    std::map<std::string, std::shared_ptr<LoxFunction>> _methods;
};

} // namespace cloxx
