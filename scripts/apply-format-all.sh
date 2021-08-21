#!/bin/bash

scriptDir="$(cd "$(dirname "$0")" && pwd -P)"
find "src" -type f -print | xargs "${scriptDir}"/apply-format.sh
find "generated_src" -type f -print | xargs "${scriptDir}"/apply-format.sh
