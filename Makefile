.PHONY: configure configure-debug ast cloxx test clean
.DEFAULT_GOAL := build/cloxx

configure:
	@scripts/configure.sh

configure-debug:
	@scripts/configure.sh --debug

ast:
	@scripts/gen-ast.sh

build/cloxx:
	@scripts/build.sh

test: build/cloxx
	@tool/run-test.py

clean:
	@rm -rf build
