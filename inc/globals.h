//
// Created by sl on 22.10.2020.
//

#ifndef YRGB2020_GLOBALS_H
#define YRGB2020_GLOBALS_H

#define MAX_SPEED 12

enum {
    G_BUF,
    G_BG_SHIFT,
    G_TRACK_POS,
    G_COARSE_POS,
    G_TURN,
    G_SPEED,
    G_ROAD_ANGLE,
    G_MY_ANGLE,
    G_MISANGLE,
    G_MISPOS,
    G_OLD_BG_SHIFT,
    G_SPRITE_X,
    G_SPRITE_Y,
    G_SPRITE_ADDR,
    G_SPRITE_POS,
    NUM_GLOBALS
};

#define A_BUF          0
#define A_BG_SHIFT     2
#define A_TRACK_POS    4
#define A_COARSE_POS   6
#define A_TURN         8
#define A_SPEED        10
#define A_ROAD_ANGLE   12
#define A_MY_ANGLE     14
#define A_MISANGLE     16
#define A_MISPOS       18
#define A_OLD_BG_SHIFT 20
#define A_SPRITE_X     22
#define A_SPRITE_Y     24
#define A_SPRITE_ADDR  26
#define A_SPRITE_POS   28

#define NUM_TURNS 19
#define CENTER_TURN 9
static const int turns[NUM_TURNS] = {32, 45, 64, 90, 128, 181, 256, 362, 512, 0, -512, -362, -256, -181, -128, -90, -64, -45, -32};

static int globals[NUM_GLOBALS];

#endif //YRGB2020_GLOBALS_H
