# cloxx ![Build Status](https://app.travis-ci.com/chanryu/cloxx.svg?branch=master)

__cloxx__ is an interpreter for [Crafting Interpreters'](http://www.craftinginterpreters.com/) [Lox](http://www.craftinginterpreters.com/the-lox-language.html) language written in C++.

__cloxx__ implements the following features:
- Recursive descent parser
- Tree-walk interpreter
- Mark-and-sweep garbage collector

## How to setup repo

Run configure script to generate Posix-compatible makefiles via CMake.
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

The test suite and test runner __cloxx__ uses was [imported](https://github.com/chanryu/cloxx/pull/2) from Crafting Interpreters, as __cloxx__ is 100% compatible with Carfting Interpreters' [jlox](https://github.com/munificent/craftinginterpreters/tree/master/java/com/craftinginterpreters/lox).

```
$ tool/run-test.py
```

## How to contribute

You don't want to. :p
