#!/bin/bash

PROJECT=$(git rev-parse --show-toplevel)
BUILD="$PROJECT/build"
if [ "$1" == "-r" ]; then
  rm -rf "$BUILD"
fi
mkdir -p "$BUILD" && pushd "$BUILD" > /dev/null && cmake -G "Unix Makefiles" .. && popd > /dev/null

"$PROJECT/scripts/gen-ast.sh"