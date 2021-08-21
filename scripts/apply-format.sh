#!/bin/bash

FILE_EXTS=".cpp .hpp"

# check whether the given file matches any of the set extensions
matches_extension() {
  local filename=$(basename "$1")
  local extension=".${filename##*.}"
  local ext

  for ext in $FILE_EXTS; do
    [[ "$ext" == "$extension" ]] && return 0;
  done

  return 1
}

CLANG_FORMAT=$(/usr/bin/which clang-format)
STYLEARG="-style=file -assume-filename=${PWD}/.clang-format"

format_file() {
  file="${1}"
  echo -n "  ${file}:"
  if matches_extension "$file"; then
    echo " applying clang-format"
    $CLANG_FORMAT -i ${STYLEARG} ${1} || {
      echo "Error executing '$CLANG_FORMAT'"
      exit 1
    }
  else
    echo " ignoring file"
  fi
}

if [ ! -f ${CLANG_FORMAT} ]; then
  echo >&2 "Install clang-format"
  exit 1
fi

if [ "$#" -lt 1 ]; then
  echo "You must supply a file list."
  exit 1
fi

for file in "$@"; do
  format_file "${file}"
done