#pragma once

#include <map>

#include "LoxObject.hpp"

namespace cloxx {

class LoxClass;
struct Token;

class LoxInstance : public LoxObject, public std::enable_shared_from_this<LoxInstance> {
public:
    LoxInstance(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass);

    std::shared_ptr<LoxClass> const& klass() const;

    std::shared_ptr<LoxObject> get(Token const& name);
    void set(Token const& name, std::shared_ptr<LoxObject> const& value);

    std::string toString() override;
    bool equals(std::shared_ptr<LoxObject> const& object) override;

    // GC support
    void enumerateTraceables(Enumerator const& enumerator) override;
    void reclaim() override;

private:
    std::shared_ptr<LoxClass> _class;
    std::map<std::string, std::shared_ptr<LoxObject>> _fields;
};

} // namespace cloxx
