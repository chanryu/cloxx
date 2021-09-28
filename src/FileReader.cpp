#include "FileReader.hpp"

#include <filesystem>

namespace cloxx {

FileReader::FileReader(std::string_view filePath)
{
    namespace fs = std::filesystem;

    auto path = fs::path{filePath};

    try {
        _filePath = fs::weakly_canonical(path).string();
    }
    catch (fs::filesystem_error&) {
        // e.g., file does not exist
        _filePath = filePath;
    }

    _ifs.open(_filePath);
    readNextChar();
}

bool FileReader::isOpen() const
{
    return _ifs.is_open();
}

std::string FileReader::filePath() const
{
    return _filePath;
}

bool FileReader::isAtEnd()
{
    return _nextChar == '\0';
}

char FileReader::readChar()
{
    char c = _nextChar;
    readNextChar();
    return c;
}

void FileReader::readNextChar()
{
    if (!_ifs.get(_nextChar)) {
        _nextChar = '\0';
    }
}

} // namespace cloxx