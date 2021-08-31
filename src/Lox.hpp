#pragma once

namespace cloxx {

class SourceReader;

class Lox {
public:
    Lox();

    int run(SourceReader& sourceReader);
    int runFile(char const* filepath);
};

} // namespace cloxx
