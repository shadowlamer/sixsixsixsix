//
// Created by sl on 22.10.2020.
//

#ifndef YRGB2020_GLOBALS_H
#define YRGB2020_GLOBALS_H

#define MAX_SPEED 8
#define NUM_MISSIONS 8
#define MISSION_LENGTH 50
#define MISSION_TIME 30
#define FPS 8

enum {
    ST_IDLE = 0,
    ST_INTRO,
    ST_MIS_INTRO,
    ST_RACE,
    ST_CRASH,
    ST_RACE_END,
    ST_QUEST,
    ST_SUCCESS,
    ST_MIS_SUCCESS,
    ST_MIS_FAIL,
    ST_FAIL,
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
    G_MISSION,
    G_TIME,
    G_TIMER,
    G_FINISH,
    G_LIVES,
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
#define A_MISSION      34
#define A_TIME         36
#define A_TIMER        38
#define A_FINISH       40
#define A_LIVES        42

#define NUM_TURNS 19
#define CENTER_TURN 9
static const int squares[64] = {0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225, 256, 289, 324, 361, 400, 441, 484, 529, 576, 625, 676, 729, 784, 841, 900, 961, 1024, 1089, 1156, 1225, 1296, 1369, 1444, 1521, 1600, 1681, 1764, 1849, 1936, 2025, 2116, 2209, 2304, 2401, 2500, 2601, 2704, 2809, 2916, 3025, 3136, 3249, 3364, 3481, 3600, 3721, 3844, 3969};
static const int turns[NUM_TURNS] = {32, 45, 64, 90, 128, 181, 256, 362, 512, 0, -512, -362, -256, -181, -128, -90, -64, -45, -32};


// ROM routines
#define ROM_OPEN_CHANNEL 0x1601             ; Open a channel
#define ROM_CLOSE        0x16e5             ; Open a channel
#define ROM_CLS          0x0DAF             ; Clear screen
#define ROM_PRINT        0x15f2             ; Print a string
// Control chars
#define INK     "\x10"
#define PAPER   "\x11"
#define FLASH   "\x12\x01"
#define NOFLASH  "\x12\x00"
#define OVER    "\x15"
#define AT      "\x16"
#define START   "\x00\x00"
#define BOTTOM  "\x14\x00"
#define BLACK   "\x00"
#define BLUE    "\x01"
#define RED     "\x02"
#define MAGENTA "\x03"
#define GREEN   "\x04"
#define CYAN    "\x05"
#define YELLOW  "\x06"
#define WHITE   "\x07"
#define EOL     "\x02"


#define FIRE    ((joystickKeysPort & 0x1f ^ 0x1f) & 0b00010000)
#define BACK    ((joystickKeysPort & 0x1f ^ 0x1f) & 0b00001000)
#define FORWARD ((joystickKeysPort & 0x1f ^ 0x1f) & 0b00000100)
#define RIGHT   ((joystickKeysPort & 0x1f ^ 0x1f) & 0b00000010)
#define LEFT    ((joystickKeysPort & 0x1f ^ 0x1f) & 0b00000001)

#define ONE   LEFT
#define TWO   RIGHT
#define THREE FORWARD
#define FOUR  BACK
#define FIVE  FIRE

#define ROAD_MARKS_NUM 2

typedef struct {
    unsigned char angle;
    unsigned char width;
    unsigned char solid;
} road_marks_t;

const road_marks_t road_marks[ROAD_MARKS_NUM] = {
    {.angle = 2, .width = 30, .solid = 1},
    {.angle = 0, .width = 0, .solid = 0}
};


#endif //YRGB2020_GLOBALS_H
