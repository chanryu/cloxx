#pragma once

#include <map>
#include <memory>
#include <string>

#include "Callable.hpp"
#include "GC.hpp"

namespace cloxx {

class LoxClass;
class LoxFunction;
class Interpreter;

struct Token;

class LoxObject : public Traceable, public std::enable_shared_from_this<LoxObject> {
public:
    LoxObject(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass);
    LoxObject(PrivateCreationTag tag, std::map<std::string, std::shared_ptr<LoxObject>> fields);
    virtual ~LoxObject();

#ifdef CLOXX_GC_DEBUG
    static size_t instanceCount();
#endif

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

std::shared_ptr<LoxClass> createObjectClass(Interpreter* interpreter);

} // namespace cloxx
