.PHONY: configure configure-debug ast cloxx test clean
.DEFAULT_GOAL := cloxx

configure:
	@scripts/configure.sh

configure-debug:
	@scripts/configure.sh --debug

ast:
	@scripts/gen-ast.sh

cloxx:
	@scripts/build.sh

test: cloxx
	@tool/run-test.py

clean:
	@rm -rf build
