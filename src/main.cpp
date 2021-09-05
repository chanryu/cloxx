#include <iostream>

#include "Lox.hpp"

#ifdef CLOXX_GC_DEBUG
#include "Assert.hpp"
#include "GC.hpp"
#include "LoxObject.hpp"
#endif

using namespace cloxx;

int main(int argc, char const* argv[])
{
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [filepath]\n";
        return 1;
    }

    int result;
    {
        Lox lox;

        if (argc == 2) {
            result = lox.runFile(argv[1]);
        }
        else {
            result = lox.repl();
        }
    }

#ifdef CLOXX_GC_DEBUG
    LOX_ASSERT(LoxObject::instanceCount() == 0);
    LOX_ASSERT(Traceable::instanceCount() == 0);
#endif

    return result;
}