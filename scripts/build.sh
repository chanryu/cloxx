#!/bin/bash

PROJECT=$(git rev-parse --show-toplevel)
BUILD="$PROJECT/build"

cmake --build "$BUILD"