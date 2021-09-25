#pragma once

#include "LoxObject.hpp"

namespace cloxx {

class LoxClass;
class Runtime;

class LoxModule : public LoxObject {
public:
    using LoxObject::LoxObject;

    std::string toString() override;

    void enumerateTraceables(Enumerator const& enumerator) override;
    void reclaim() override;

    std::shared_ptr<Environment> env;
};

std::shared_ptr<LoxClass> createModuleClass(Runtime* runtime);

} // namespace cloxx
