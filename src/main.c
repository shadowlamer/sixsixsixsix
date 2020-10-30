#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "dashboard.h"
#include "background1.h"
#include "background2.h"
#include "track.h"
#include "sprites.h"
#include "crash.h"
#include "messages.h"
#include "font1.h"

#define SCREEN_BUFFER_START 0x4000
#define ROAD_SCREEN_BUFFER_START 0x4800
#define DASH_SCREEN_BUFFER_START 0x5000

__at (SCREEN_BUFFER_START) char screen_buf[0x1800];
__at (ROAD_SCREEN_BUFFER_START) char screen_road_buf[0x800];
__at (DASH_SCREEN_BUFFER_START) char screen_dash_buf[0x800];
__at (0x5c36) unsigned int *font_pointer;
__sfr __at 0xfe joystickKeysPort;

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

static int shifts[64];
static char double_buf[0x810];
static int squares[64];
static unsigned char scanline;
static char sprite_buf[48];

void render_road();
void mark(int pos, unsigned char line, unsigned char *pp);
void calc_shifts();
void render_background();
void render_dashboard();
void render_map();
void render_pos(unsigned int pos);
void plot(unsigned char x, unsigned char y);
void render_sprite();
unsigned int random();
void cls();

int main() {
  unsigned char i;

  globals[G_TRACK_POS] = 0;
  globals[G_COARSE_POS] = 0;
  globals[G_SPEED] = 0;
  globals[G_SPEED] = 0;
  globals[G_ROAD_ANGLE] = track[globals[G_COARSE_POS]].t;
  globals[G_MY_ANGLE] = globals[G_ROAD_ANGLE];
  globals[G_MISPOS] = 0;
  globals[G_OLD_BG_SHIFT] = 0xff;
  globals[G_SPRITE_POS] = 0;
  globals[G_STATE] = ST_IDLE;
  globals[G_SPRITE_Y] = 0;

  for (i=0;i<64;i++) squares[i] = i*i;

//  render_dashboard();

  cls();
  printf(msg_intro1);

  while (1) {

    switch(globals[G_STATE]) {
      case ST_IDLE:
        break;
      case ST_RACE:
        scanline = joystickKeysPort & 0x0f ^ 0x0f;
        if (scanline & 0b00000100) globals[G_SPEED]++;
        if (scanline & 0b00001000) globals[G_SPEED]--;
        if (scanline & 0b00000010) globals[G_MY_ANGLE]--;
        if (scanline & 0b00000001) globals[G_MY_ANGLE]++;
        if (globals[G_SPEED] > MAX_SPEED) globals[G_SPEED] = MAX_SPEED;
        if (globals[G_SPEED] < 0) globals[G_SPEED] = 0;

        globals[G_TRACK_POS] += globals[G_SPEED];
        globals[G_COARSE_POS] = globals[G_TRACK_POS] >> 6;
        if (globals[G_COARSE_POS] >= TRACK_SIZE) globals[G_TRACK_POS] = 0;
        globals[G_TURN] = track[globals[G_COARSE_POS] + AHEAD].t;
        globals[G_ROAD_ANGLE] = track[globals[G_COARSE_POS]].t;
        globals[G_MISANGLE] = globals[G_MY_ANGLE] - globals[G_ROAD_ANGLE];
        globals[G_MISPOS] += ((globals[G_SPEED] * globals[G_MISANGLE]) >> 2);

//    render_pos(globals[G_COARSE_POS]);
        calc_shifts();

        globals[G_SPRITE_Y] += globals[G_SPEED];

        globals[G_SPRITE_ADDR] = (unsigned int) (&sprites[(globals[G_SPRITE_Y] >> 3) & 0x07]);
        if (globals[G_SPRITE_Y] < 64) {
          switch (globals[G_SPRITE_POS]) {
            case 0:
              globals[G_SPRITE_X] = shifts[globals[G_SPRITE_Y]] - globals[G_SPRITE_Y] - 10;
              break;
            case 1:
              globals[G_SPRITE_X] = shifts[globals[G_SPRITE_Y]];
              break;
            case 2:
              globals[G_SPRITE_X] = shifts[globals[G_SPRITE_Y]] + globals[G_SPRITE_Y] + 10;
              break;
          }
        } else {
          if (globals[G_SPRITE_X] > (127 - 20) && globals[G_SPRITE_X] < (127 + 20))  {
            globals[G_MY_ANGLE] += globals[G_TURN] << 2;
          }
          globals[G_SPRITE_Y] = 0;
          globals[G_SPRITE_POS] = random() % 3;
        }
        render_background();
        render_road();
//    render_pos(globals[G_COARSE_POS]);

        if (globals[G_MISPOS] > 150 || globals[G_MISPOS] < -150) {
          globals[G_STATE] = ST_CRASH;
          memcpy(screen_buf, bin2c_crash_bin, 0x1800);
        }

        break;
    }
  }
  return 0;
}

void calc_shifts() {
  char i;
  int factor = turns[globals[G_TURN] + CENTER_TURN];
  for (i=0; i<64; i++) {
    shifts[63 - i] = 127 + globals[G_MISPOS] + squares[i] / factor;
  }
}

void mark(int pos, unsigned char line, unsigned char *pp) {
  unsigned char fill_buf;
  unsigned int x;
  x = pos + shifts[line];
  if (x > 0xff) return;
  fill_buf = 0xc0 >> (x & 0x07);
  pp[x / 8] |= fill_buf;
}

void render_road() {
  unsigned int line;
  unsigned char w;
  unsigned char *p;
  unsigned char m;
  memset(double_buf, 0x00, 0x0800);
  for (line = 0; line < 64; line++) {
    p = double_buf + ((line & 0x07) << 8) + ((line & 0x38) << 2);
    for (m = 0; m < ROAD_MARKS_NUM; m++) {
      if (road_marks[m].solid || (((line >> 3) & 0x03) == ((globals[G_TRACK_POS] >> 3) & 0x03))) {
        w = line * road_marks[m].angle;
        mark(+road_marks[m].width + w, line, p);
        if (road_marks[m].width == 0) break;
        mark(-road_marks[m].width - w, line, p);
      }
    }
  }
  render_sprite();
  memcpy(screen_road_buf, double_buf, 0x800);
}

void render_dashboard() {
  memcpy(screen_dash_buf, bin2c_dashboard_bin, 0x800);
//  render_map();
}

void render_background(){
  __asm
  ld iy, #_globals

  ld e, A_MY_ANGLE+0(iy)
  ld d, A_MY_ANGLE+1(iy)
  ld c, A_SPEED+0(iy)
  ld b, A_SPEED+1(iy)
  call #__mul16                  ; Multiply speed by turn angle
  ld e, A_BG_SHIFT+0(iy)
  ld d, A_BG_SHIFT+1(iy)
  add hl, de                     ; Increase global background shift
  ld A_BG_SHIFT+0(iy), l
  ld A_BG_SHIFT+1(iy), h         ; And put it back

  rr h                           ; Calc image shift
  rr l
  rr h
  rr l
  rr h
  rr l
  rr h
  rr l
  rr l
  rr l
  rr l

  xor a
  sub l                          ; Invert direction
  and #0x1f                      ; Limit image shift to screen width

  cp A_OLD_BG_SHIFT(iy)
  jr z, render_background_end     ; Background not changed

  ld A_OLD_BG_SHIFT(iy), a       ; Store image shift in a buffer

  ld c, a
  ld b, #0
  ld de, #_screen_buf           ; Load screen buffer address
  ld hl, #_bin2c_background1_bin   ; Load background start address
  add hl, bc                     ; Shift image

  ld b, #0x40                    ; Load lines number
  render_background_loop:          ; Loop by lines

  push bc                        ; Store lines loop counter

  ld b, #0
  ld a, #0x20
  sbc a, A_OLD_BG_SHIFT(iy)               ; Calc number of bytes in first part
  jp z, skip_ldir_1              ; Do not copy zero bytes
  ld c, a
  ldir                           ; Copy first part
  skip_ldir_1:

  ld bc, #0x0020
  sbc hl, bc
  ld b, #0
  xor a
  add a, A_OLD_BG_SHIFT(iy)          ; Calc number of bytes in second part
  jp z, skip_ldir_2              ; Do not copy zero bytes
  ld c, a
  ldir                           ; Copy second part
  skip_ldir_2:

  ld bc, #0x0020                 ; Shift image to next line
  add hl, bc

  pop bc                         ; Restore lines loop counter
  djnz render_background_loop
  render_background_end:
  __endasm;
}

void plot(unsigned char x, unsigned char y) {
  __asm
  ld iy,#2
  add iy,sp                      ; Bypass the return address of the function
  ld c, 0(iy)
  ld b, 1(iy)
  call #0x22aa                   ; Pixel addr subroutine
  ld b, a
  ld a, #0x80
  plot_loop:
  rrca                           ; Move pixel to the right position in byte
  djnz plot_loop
  xor a, (hl)
      ld (hl), a                     ; Draw pixel
  __endasm;
}

void render_pos(unsigned int pos) {
  unsigned char x = track[pos].x;
  unsigned char y = track[pos].y;
  plot(80 + x, 0 + y);
}

void render_map() {
  unsigned char i;
  for (i=0;i<TRACK_SIZE;i++) render_pos(i);
}

void render_sprite() {
  __asm
  push af
  ld iy, #_globals

  ld e, A_SPRITE_ADDR+0(iy)
  ld d, A_SPRITE_ADDR+1(iy)
  ld hl, #_sprite_buf           ; Copy from sprite to buffer

  ld c, #16
  copy_sprite_loop:               ; Loop by lines of sprite

  inc hl                        ; HL to second byte of buffer

  xor a
  ld (hl), a                    ; Clean sprite buffer

  ld a, A_SPRITE_X(iy)    ; Load X pos
  and #0x07                     ; Extract shift part of X pos
  jr nz, ll1
  ld a, (de)
      jr skip_l1
  ll1:
  ld b, a                       ; Init shift loop
  ld a, (de)                    ; Load first byte of sprite line
  l1:
  srl a
  rr (hl)
  djnz l1                       ; Shift first byte of sprite line to second byte of buffer line
  skip_l1:
  dec hl                        ; Rewind HL to first byte of buffer line
  ld (hl), a                    ; Remain of first byte to first byte of buffer
  inc hl                        ; HL to second byte of buffer line
  inc de                        ; DE to second byte of sprite line

  inc hl                        ; HL to third byte of sprite line

  xor a
  ld (hl), a                    ; Clean sprite buffer

  ld a, A_SPRITE_X(iy)    ; Load X pos
  and #0x07                     ; Extract shift part of X pos
  jr nz, ll2
  ld a, (de)
  jr skip_l2
  ll2:
  ld b, a                       ; Init shift loop
  ld a, (de)                    ; Load second byte of sprite line
  l2:
  srl a
  rr (hl)
  djnz l2                      ; Shift second byte of sprite line to third byte of buffer line
  skip_l2:
  dec hl                       ; Rewind HL to first byte of buffer line
  or (hl)                      ; Combine remain of second byte with part of first byte in buffer
  ld (hl), a                   ; Put result back to buffer
  inc hl                       ; HL to second byte of buffer line

  inc de                       ; Next line of sprite
  inc hl                       ; Next line of buffer

  dec c
  jr nz, copy_sprite_loop      ; End of loop by lines

  ld de, #_sprite_buf


  ld a, #64
  sub A_SPRITE_Y(iy)
  jr c, sprite_end

  ld c, #0
  ld b, #16
  cp b
  jr nc, render_sprite_inner_loop
  ld b, a
  render_sprite_inner_loop:

  ld hl, #_double_buf

  ld a, A_SPRITE_X(iy)
  srl a
  srl a
  srl a
  add a, l
  ld l, a

  ld a, c
  add A_SPRITE_Y(iy)
  and #0x07
  add a, h
  ld h, a

  ld a, c
  add A_SPRITE_Y(iy)
  add a, a
  add a, a
  and #0xe0
  add a, l
  ld l, a

  ld a, h
  adc a, #0
  ld h, a

  ld a, A_SPRITE_X+1(iy)
  or a
  jr nz, skip1
  ld a, (de)
  or (hl)
  ld (hl), a
  skip1:
  inc hl
  inc de

  ld a, A_SPRITE_X+1(iy)
  or a
  jr nz, skip2
  ld a, A_SPRITE_X+0(iy)
  cp #0xf8
  jr nc, skip2
  ld a, (de)
  or (hl)
  ld (hl), a
  skip2:
  inc hl
  inc de

  ld a, A_SPRITE_X+1(iy)
  or a
  jr nz, skip3
  ld a, A_SPRITE_X(iy)
  cp #0xf0
  jr nc, skip3
  ld a, (de)
  or (hl)
  ld (hl), a
  skip3:
  inc de

  inc c
  djnz render_sprite_inner_loop
  sprite_end:
  pop af
  __endasm;
}

unsigned int random() {
  __asm
  ld iy, #_globals
  ld l,A_SEED+0(iy)
  ld h,A_SEED+1(iy)
  ld a,r
  ld d,a
  ld e,(hl)
      add hl,de
  add a,l
  xor h
  ld A_SEED+0(iy),l
  ld A_SEED+1(iy),h
  __endasm;
}

void print(int c) {
  __asm
  ld iy, #2
  add iy, sp
  ld a, 0(iy)
  rst #0x10
  __endasm;
}

int putchar(int c) {
  if (c >= 0xc0) {
    c -= 0x80;
    font_pointer = (font1 - 0x200);
  } else {
    font_pointer = 0x3c00;
  }
  print(c);
  return c;
}

void cls() {
  __asm
  call #ROM_CLS
  __endasm;
}