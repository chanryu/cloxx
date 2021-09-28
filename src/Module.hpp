#pragma once

#include <map>
#include <string>

#include "GC.hpp"

namespace cloxx {

class LoxObject;

class Module : public Traceable {
public:
    Module(PrivateCreationTag, std::map<std::string, std::shared_ptr<LoxObject>> const& values);

    std::shared_ptr<LoxObject> get(std::string const& name) const;
    void set(std::string const& name, std::shared_ptr<LoxObject> const& value);

    // GC support
    void enumerateTraceables(Traceable::Enumerator const& enumerator) override;
    void reclaim() override;

private:
    std::map<std::string, std::shared_ptr<LoxObject>> _values;
};

} // namespace cloxx
