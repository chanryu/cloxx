#pragma once

#include <fstream>
#include <string>

#include "SourceReader.hpp"

namespace cloxx {
class FileReader : public SourceReader {
public:
    explicit FileReader(std::string const& filePath);

    bool isOpen() const;

    bool isAtEnd() override;
    char readChar() override;

private:
    void readNextChar();

    std::ifstream _ifs;
    char _nextChar;
};
} // namespace cloxx