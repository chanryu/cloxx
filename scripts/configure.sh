#!/bin/bash

#set -e

PROJECT=$(git rev-parse --show-toplevel)
BUILD="$PROJECT/build"

BUILD_TYPE_FILE_PATH="$BUILD/BUILD_TYPE"
BUILD_TYPE="Release"
OLD_BUILD_TYPE=""

CLEAN_FORCED=no

while [[ $# -gt 0 ]]; do
  case "$1" in
    -d|--debug)
      BUILD_TYPE="Debug"
      shift
      ;;
    -c|--clean)
      CLEAN_FORCED=yes
      shift
      ;;
    *) # unknown option - skip
      shift
      ;;
  esac
done


buildtype_mismatch() {
  if [ -f "$BUILD_TYPE_FILE_PATH" ]; then
    OLD_BUILD_TYPE=`cat $BUILD_TYPE_FILE_PATH`
    if [ "$OLD_BUILD_TYPE" == $BUILD_TYPE ]; then
      return 1
    fi
  fi
  return 0
}

if [ $CLEAN_FORCED == "yes" ] || buildtype_mismatch ; then
  rm -rf "$BUILD"
fi

# generate Makefiles
mkdir -p "$BUILD" && pushd "$BUILD" && cmake  -DCMAKE_BUILD_TYPE=$BUILD_TYPE -G "Unix Makefiles" .. && popd > /dev/null

# store current build type
printf "$BUILD_TYPE" > "$BUILD_TYPE_FILE_PATH"