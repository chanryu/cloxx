# cloxx

__cloxx__ is an interpreter for [Crafting Interpreters'](http://www.craftinginterpreters.com/) [Lox](http://www.craftinginterpreters.com/the-lox-language.html) language written in C++.

__cloxx__ implements the following features:
- Recursive descent parser
- Tree-walk interpreter
- Mark-and-sweep garbage collector

## How to build cloxx

Once you've cloned the repo, run bootstrapping script to setup the repo, e.g., generate AST files, etc.:
```bash
$ script/bootstrap.sh
```

Then configure build files. By default, it will generate Posix-compatible makefiles.
```bash
$ script/configure.sh # pass "--debug" if that's what you want
$ script/build.sh
```

Finally, build the cloxx interpreter. This will create cloxx executable under `build` directory.
```bash
$ script/build.sh
```

## How to run test suite

```
$ tool/run-test.py [cloxx]
```

## How to contribute

You don't want to. :p