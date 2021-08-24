# cloxx ![Build Status](https://app.travis-ci.com/chanryu/cloxx.svg?branch=master)

__cloxx__ is an interpreter for [Crafting Interpreters'](http://www.craftinginterpreters.com/) [Lox](http://www.craftinginterpreters.com/the-lox-language.html) language written in C++.

As a fully hand-written interpreter, __cloxx__ implements the following features:

- Recursive descent parser
- Tree-walk interpreter
- Mark-and-sweep garbage collector

## Running cloxx

### Setup project

Once you've cloned the repository, you should run the configuration script to setup project. It will generate Posix-compatible makefiles via CMake.

```bash
$ script/configure.sh  # pass "--debug" if that's what you want
```

As well as the source files checked into the repo, you will also need to generate some files to build the project. You won't need to do it again unless you're hacking — i.e., modifying Abstract Syntax Tree — __cloxx__.

```bash
$ script/gen-ast.sh
```

### Build

Finally, we're all set to build the project.

```bash
$ script/build.sh
```

If everthing goes well, you should have an executable `cloxx` under the `build` directory.

### Test

While I didn't put much effort on creating tests, I made __cloxx__ fully compatible with __jlox__ from [Crafting Interpreters](https://github.com/munificent/craftinginterpreters). This means I can reuse __jlox__'s comprehensive test suites. The following Python script was [ported (and simplified for __cloxx__)](https://github.com/chanryu/cloxx/pull/2) from [the Crafting Interpreters' test runner written Dart](https://github.com/munificent/craftinginterpreters/blob/master/tool/bin/test.dart).

```bash
$ tool/run-test.py
```

## Repository layout

- `build/` - Intermediate files and other build output go here. Not committed to Git.
- `gen/` – C++ source files generated by `gen-ast.py`. Not committed.
- `src/` – Most of the C++ source files.
- `test/` - Test suite files imported from [Crafting Interpreters](https://github.com/munificent/craftinginterpreters).
- `tool/` - Mostly, Python scripts for generating files and unit testing.
- `scripts/` - Shell scripts to configure the projects, wrapper of the above Python scripts, and etc.
