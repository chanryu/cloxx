#pragma once

#include <memory>
#include <string>

namespace cloxx {

class ScriptReader {
public:
    virtual ~ScriptReader() = default;

    virtual bool isAtEnd() = 0;
    virtual char readChar() = 0;
};

} // namespace cloxx
