#include "Module.hpp"

#include "Assert.hpp"
#include "LoxObject.hpp"

namespace cloxx {

Module::Module(PrivateCreationTag tag, std::map<std::string, std::shared_ptr<LoxObject>> const& values)
    : Traceable{tag}, _values{values}
{}

std::shared_ptr<LoxObject> Module::get(std::string const& name) const
{
    if (auto it = _values.find(name); it != _values.end()) {
        return it->second;
    }

    return nullptr;
}

void Module::set(std::string const& name, std::shared_ptr<LoxObject> const& value)
{
    LOX_ASSERT(value);
    _values[name] = value;
}

void Module::enumerateTraceables(Traceable::Enumerator const& enumerator)
{
    for (auto& [_, value] : _values) {
        enumerator.enumerate(*value);
    }
}

void Module::reclaim()
{
    _values.clear();
}

} // namespace cloxx
