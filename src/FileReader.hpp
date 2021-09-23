#pragma once

#include <fstream>
#include <string>

#include "ScriptReader.hpp"

namespace cloxx {
class FileReader : public ScriptReader {
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