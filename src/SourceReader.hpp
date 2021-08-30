#pragma once

#include <memory>
#include <string>

namespace cloxx {

class SourceReader {
public:
    virtual ~SourceReader() = default;

    virtual bool isEndOfFile() = 0;
    virtual void readSource(std::string& source) = 0;
};

} // namespace cloxx
