# cloxx ![Build Status](https://app.travis-ci.com/chanryu/cloxx.svg?branch=master)

__cloxx__ is an C++ implementation of [Lox](http://www.craftinginterpreters.com/the-lox-language.html), the language featured in [Crafting Interpreters](http://www.craftinginterpreters.com/).

__cloxx__ is a purely educational project for myself and thus it won't be vrery useful for building real-world applications. However, if you're another reader of the book and implementing Lox in C++, __cloxx__ might be an interesting reference to you. While it's fully _C++ish_, I also tried to make it as close as possible to the book's __jlox__ implementation in terms of identifiers such as class, variable and function names. It should be easy to follow along with the book.

Aside form the techniques explain the book such as Recursive Descent Parsing and Tree-Walk Interpretation, __cloxx__ implements Mark-and-Sweep Garbage Collection.

## Running cloxx

While I'm not willing to spend more time on building the project in different environments, the following instructions work for my MacBook and would also work in most Posix-compatible environments where [CMake](https://cmake.org), bash and a decent C++17 compiler are available.

### Setup project

Once you've cloned the repo, you should run the configuration script to setup the project. It will generate Posix-compatible makefiles via CMake.

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

While I didn't put much effort on creating tests, __cloxx__ is fully compatible with __jlox__. This means I can just reuse __jlox__'s comprehensive test suites. The following Python script runs the full jlox test suites imported from the Crafting Interpreters repo.

```bash
$ tool/run-test.py
```

## Directory layout

- `build/` - Intermediate files and other build output go here. Not committed to Git.
- `gen/` – C++ source files generated by `gen-ast.py`. Not committed.
- `src/` – Most of the C++ source files.
- `test/` - Test suite files imported from [Crafting Interpreters](https://github.com/munificent/craftinginterpreters).
- `tool/` - Mostly, Python scripts for generating files and unit testing.
- `scripts/` - Shell scripts to configure the projects, wrapper of the above Python scripts, and etc.
