#pragma once

#include <map>

#include "GC.hpp"
#include "LoxObject.hpp"

namespace cloxx {

class LoxClass;
enum class LoxClassId : size_t;

struct Token;

class LoxInstance : public LoxObject, public Traceable, public std::enable_shared_from_this<LoxInstance> {
public:
    LoxInstance(PrivateCreationTag tag, std::shared_ptr<LoxClass> const& klass);

    std::shared_ptr<LoxObject> get(Token const& name);
    void set(Token const& name, std::shared_ptr<LoxObject> const& value);

    std::shared_ptr<Traceable> getNativeData(LoxClassId classId) const;

    std::string toString() override;

    // GC support
    void enumerateTraceables(Enumerator const& enumerator) override;
    void reclaim() override;

private:
    std::shared_ptr<LoxClass> _class;
    std::map<std::string, std::shared_ptr<LoxObject>> _fields;
    std::map<LoxClassId, std::shared_ptr<Traceable>> _nativeDataMap;
};

} // namespace cloxx
