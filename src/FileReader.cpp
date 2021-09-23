#include "FileReader.hpp"

namespace cloxx {

FileReader::FileReader(std::string const& filePath) : _ifs{filePath}
{
    readNextChar();
}

bool FileReader::isOpen() const
{
    return _ifs.is_open();
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