#!/bin/bash

BUILD_DIR=${1:-cmake-build-debug}

PROJECT="6.6.6.6"
TAPE="${PWD}/sixsixsixsix.tap"
CODE="${PWD}/${BUILD_DIR}/yrgb2020.bin"
INTRO="${PWD}/assets/intro.scr"

zmakebas -i 1 -a 10 -l -o ${TAPE} -n ${PROJECT} loader.bas

# Install dependencies
[ -d tools/jsbin2tap/node_modules ] || npm --prefix tools/jsbin2tap install

npm --prefix tools/jsbin2tap  start -- -p -o ${TAPE} ${INTRO} ${CODE}
