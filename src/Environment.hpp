#pragma once

#include <map>
#include <memory>
#include <string>

#include "Traceable.hpp"

namespace cloxx {

class LoxObject;
struct Token;

class Environment : public Traceable {
public:
    Environment() = default;
    explicit Environment(std::shared_ptr<Environment> const& enclosing);

    void define(std::string const& name, std::shared_ptr<LoxObject> const& value);

    std::shared_ptr<LoxObject> get(Token const& name) const;
    void assign(Token const& name, std::shared_ptr<LoxObject> const& value);

    std::shared_ptr<LoxObject> getAt(size_t distance, std::string const& name) const;
    void assignAt(size_t distance, std::string const& name, std::shared_ptr<LoxObject> const& value);

    // GC support
    void mark() override;

private:
    Environment const* ancestor(size_t distance) const;
    Environment* ancestor(size_t distance);

    std::shared_ptr<Environment> const _enclosing;
    std::map<std::string, std::shared_ptr<LoxObject>> _values;
};

} // namespace cloxx
