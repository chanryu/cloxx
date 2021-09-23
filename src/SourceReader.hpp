#pragma once

#include <memory>
#include <string>

namespace cloxx {

class SourceReader {
public:
    virtual ~SourceReader() = default;

    virtual bool isAtEnd() = 0;
    virtual char readChar() = 0;
};

} // namespace cloxx
