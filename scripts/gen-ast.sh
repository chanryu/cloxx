#!/bin/bash

PROJECT=$(git rev-parse --show-toplevel)
GENPATH="$PROJECT/gen/ast"

rm -rf "$GENPATH"
mkdir -p "$GENPATH"
"$PROJECT/tool/gen-ast.py" "$GENPATH"
