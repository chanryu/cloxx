#pragma once

#include <map>
#include <memory>
#include <string>

#include "GC.hpp"

namespace cloxx {

class LoxObject;
struct Token;

class Environment : public Traceable {
public:
    Environment(Traceable::CreationTag);
    Environment(Traceable::CreationTag, std::shared_ptr<Environment> const& enclosing);

    void define(std::string const& name, std::shared_ptr<LoxObject> const& value);

    std::shared_ptr<LoxObject> get(Token const& name) const;
    void assign(Token const& name, std::shared_ptr<LoxObject> const& value);

    std::shared_ptr<LoxObject> getAt(size_t distance, std::string const& name) const;
    void assignAt(size_t distance, std::string const& name, std::shared_ptr<LoxObject> const& value);

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

private:
    Environment const* ancestor(size_t distance) const;
    Environment* ancestor(size_t distance);

    std::shared_ptr<Environment> _enclosing;
    std::map<std::string, std::shared_ptr<LoxObject>> _values;
};

} // namespace cloxx
