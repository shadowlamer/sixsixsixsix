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
track_converter assets/track?.svg -d inc -p __YRGB2020

# Process sprites
sprites_extractor -n sprites -p __YRGB2020 -w 2 -h 16 -o inc/sprites.h assets/sprites.scr
sprites_extractor -n map_sprites -p __YRGB2020 -w 11 -h 48 -o inc/mapsprites.h assets/mapsprites1.scr assets/mapsprites2.scr
