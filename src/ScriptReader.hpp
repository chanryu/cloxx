#pragma once

#include <memory>
#include <string>

namespace cloxx {

class ScriptReader {
public:
    virtual ~ScriptReader() = default;

    virtual std::string filePath() const = 0;
    virtual bool isAtEnd() = 0;
    virtual char readChar() = 0;
};

} // namespace cloxx
