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

# Process images
for IMG in $IMAGES; do
  dd if=assets/${IMG}.scr of=assets/${IMG}.bin bs=2048 count=1
  bin2c -G __${IMG^^}_H -H inc/${IMG}.h assets/${IMG}.bin
  rm -f assets/${IMG}.bin
done

# Process tracks
npm --prefix tools/track_converter start -- ${PWD}/assets/track?.svg -d ${PWD}/inc -p __YRGB2020

# Process sprites
npm --prefix tools/sprites_extractor start -- -n sprites -p __YRGB2020 -w 2 -h 16 -o ${PWD}/inc/sprites.h ${PWD}/assets/sprites.scr
npm --prefix tools/sprites_extractor start -- -n map_sprites -p __YRGB2020 -w 11 -h 48 -o ${PWD}/inc/mapsprites.h ${PWD}/assets/mapsprites1.scr ${PWD}/assets/mapsprites2.scr
