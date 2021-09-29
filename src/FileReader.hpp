#pragma once

#include <fstream>
#include <string>
#include <string_view>

#include "ScriptReader.hpp"

namespace cloxx {
class FileReader : public ScriptReader {
public:
    explicit FileReader(std::string_view filePath);

    bool isOpen() const;

    std::string filePath() const override;
    bool isAtEnd() override;
    char readChar() override;

private:
    void readNextChar();

    std::string _filePath;
    std::ifstream _ifs;
    char _nextChar;
};
} // namespace cloxx