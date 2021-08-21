#!/bin/bash

PROJECT=$(git rev-parse --show-toplevel)
GENPATH="$PROJECT/generated_src/ast"

rm -rf "$GENPATH"
mkdir -p "$GENPATH"
"$PROJECT/scripts/gen-ast.py" "$GENPATH"
