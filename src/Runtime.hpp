#pragma once

#include <map>

#include "GC.hpp"

namespace cloxx {

class LoxClass;
class LoxObject;

class Module;

class Runtime {
public:
    Runtime();

    template <typename T, typename... Args>
    std::shared_ptr<T> create(Args&&... args)
    {
        return _gc.create<T>(this, std::forward<Args>(args)...);
    }

    std::shared_ptr<Environment> createEnvironment(std::shared_ptr<Environment> const& closure);
    std::shared_ptr<Module> createModule(std::map<std::string, std::shared_ptr<LoxObject>> const& values);

    std::shared_ptr<Environment> const& root();
    void collectGarbage();

    std::shared_ptr<LoxClass> const& objectClass();
    std::shared_ptr<LoxClass> const& functionClass();

    std::shared_ptr<LoxObject> getNil();
    std::shared_ptr<LoxObject> toLoxBool(bool value);
    std::shared_ptr<LoxObject> toLoxNumber(double value);
    std::shared_ptr<LoxObject> toLoxString(std::string value);

private:
    GarbageCollector _gc;

    // Built-in classes
    std::shared_ptr<LoxClass> _objectClass;
    std::shared_ptr<LoxClass> _functionClass;
    std::shared_ptr<LoxClass> _boolClass;
    std::shared_ptr<LoxClass> _numberClass;
    std::shared_ptr<LoxClass> _stringClass;
    std::shared_ptr<LoxClass> _listClass;
    std::shared_ptr<LoxClass> _moduleClass;

    // Built-in instances
    std::shared_ptr<LoxObject> _nil;
    std::shared_ptr<LoxObject> _true;
    std::shared_ptr<LoxObject> _false;
};

} // namespace cloxx