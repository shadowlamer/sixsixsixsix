#!/bin/bash

PROJECT="6.6.6.6"
TAPE="dist/tap/program.tap"
CODE="cmake-build-debug/yrgb2020.bin"

zmakebas -i 1 -a 10 -l -o ${TAPE} -n ${PROJECT} loader.bas
jsbin2tap -p -o ${TAPE} assets/intro.scr ${CODE}
