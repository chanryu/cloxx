#include <fstream>
#include <iostream>
#include <streambuf>

#include "Assert.hpp"
#include "GC.hpp"
#include "Lox.hpp"
#include "LoxObject.hpp"

using namespace cloxx;

int main(int argc, char const* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filepath\n";
        return 1;
    }

    std::ifstream ifs{argv[1]};
    if (!ifs.is_open()) {
        std::cerr << "Error: Cannot open file '" << argv[1] << "' to read!\n";
        return 1;
    }

    std::string source{std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};

    int result;
    {
        Lox lox;
        result = lox.run(source);
    }

#ifdef CLOXX_GC_DEBUG
    LOX_ASSERT(LoxObject::instanceCount() == 0);
    LOX_ASSERT(Traceable::instanceCount() == 0);
#endif

    return result;
}