#!/bin/bash

ASSETS=assets
INC=inc

IMAGES="\
background1
crash
dashboard
mis_fail
mis_success
task
task_success
success
fail
"

for IMG in $IMAGES; do
  dd if=assets/${IMG}.scr of=assets/${IMG}.bin bs=2048 count=1
  bin2c -G __${IMG^^}_H -H inc/${IMG}.h assets/${IMG}.bin
  rm -f assets/${IMG}.bin
done