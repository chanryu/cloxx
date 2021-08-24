# cloxx

__cloxx__ is an interpreter for [Crafting Interpreters'](http://www.craftinginterpreters.com/) [Lox](http://www.craftinginterpreters.com/the-lox-language.html) language written in C++.

__cloxx__ implements the following features:
- Recursive descent parser
- Tree-walk interpreter
- Mark-and-sweep garbage collector

## How to setup repo

Then configure build files. By default, it will generate Posix-compatible makefiles via CMake.
```bash
$ script/configure.sh # pass "--debug" if that's what you want
```

## Building cloxx

Generate AST files. This needs to be done only once after cloning the repo, unless you're hacking __cloxx__.
```bash
$ script/gen-ast.sh
```

Run the build script located in the `scripts` directory. If everything goes well, this will create a executable `cloxx` under the `build` directory.
```bash
$ script/build.sh
```

## How to run test suite

```
$ tool/run-test.py
```

## How to contribute

You don't want to. :p
