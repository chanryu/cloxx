#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
$SCRIPT_DIR/configure.sh --debug $1