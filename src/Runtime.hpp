#pragma once

#include "GC.hpp"

namespace cloxx {

class LoxClass;
class LoxObject;

class Runtime {
public:
    Runtime();

    template <typename T, typename... Args>
    std::shared_ptr<T> create(Args&&... args)
    {
        return _gc.create<T>(std::forward<Args>(args)...);
    }

    std::shared_ptr<LoxClass> objectClass();
    std::shared_ptr<LoxClass> functionClass();

    std::shared_ptr<LoxObject> makeLoxNil();
    std::shared_ptr<LoxObject> toLoxBool(bool value);
    std::shared_ptr<LoxObject> toLoxNumber(double value);
    std::shared_ptr<LoxObject> toLoxString(std::string value);

    // protected:
    GarbageCollector _gc;

private:
    // Built-in classes
    std::shared_ptr<LoxClass> _objectClass;
    std::shared_ptr<LoxClass> _functionClass;
    std::shared_ptr<LoxClass> _nilClass;
    std::shared_ptr<LoxClass> _boolClass;
    std::shared_ptr<LoxClass> _numberClass;
    std::shared_ptr<LoxClass> _stringClass;
    std::shared_ptr<LoxClass> _listClass;
};

} // namespace cloxx