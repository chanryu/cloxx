#pragma once

#include <map>
#include <memory>
#include <string>

#include "Callable.hpp"
#include "GC.hpp"

namespace cloxx {

class LoxClass;
class LoxFunction;
class Runtime;

struct Token;

class LoxObject : public Traceable, public std::enable_shared_from_this<LoxObject> {
private:
    // A constructor for LoxClass where `klass` is missing.
    // All other classes should use the other constructor.
    friend class LoxClass;
    LoxObject(PrivateCreationTag tag);

public:
    LoxObject(PrivateCreationTag tag, Runtime* runtime, std::shared_ptr<LoxClass> const& klass);
    virtual ~LoxObject() = default;

    std::shared_ptr<LoxObject> get(Token const& name);
    void set(Token const& name, std::shared_ptr<LoxObject> const& value);

    virtual std::string toString();
    virtual bool isTruthy();
    virtual bool equals(std::shared_ptr<LoxObject> const& object);

    // GC support
    void enumerateTraceables(Enumerator const& enumerator) override;
    void reclaim() override;

private:
    std::shared_ptr<LoxClass> _class;
    std::map<std::string, std::shared_ptr<LoxObject>> _fields;
};

std::shared_ptr<LoxClass> createObjectClass(Runtime* interpreter);

} // namespace cloxx
