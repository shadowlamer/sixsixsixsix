//
// Created by sl on 22.10.2020.
//

#ifndef YRGB2020_GLOBALS_H
#define YRGB2020_GLOBALS_H

#define MAX_SPEED 12

enum {
    ST_IDLE = 0,
    ST_RACE,
    ST_CRASH,
    NUM_STATES
};

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
    G_SEED,
    G_STATE,
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
#define A_SEED         30
#define A_STATE        32

#define NUM_TURNS 19
#define CENTER_TURN 9
static const int turns[NUM_TURNS] = {32, 45, 64, 90, 128, 181, 256, 362, 512, 0, -512, -362, -256, -181, -128, -90, -64, -45, -32};

static int globals[NUM_GLOBALS];

// ROM routines
#define ROM_OPEN_CHANNEL 0x1601             ; Open a channel
#define ROM_CLS          0x0DAF

// Control chars
#define INK     "\x10"
#define PAPER   "\x11"
#define OVER    "\x15"
#define AT      "\x16"
#define START   "\x01\x01"
#define BLACK   "\x00"
#define BLUE    "\x01"
#define RED     "\x02"
#define MAGENTA "\x03"
#define GREEN   "\x04"
#define CYAN    "\x05"
#define YELLOW  "\x06"
#define WHITE   "\x07"

#endif //YRGB2020_GLOBALS_H
