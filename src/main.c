#include <string.h>

#include "dashboard.h"
#include "track.h"

#define SCREEN_BUFFER_START 0x4000
#define ROAD_SCREEN_BUFFER_START 0x4800
#define DASH_SCREEN_BUFFER_START 0x5000

__at (SCREEN_BUFFER_START) char screen_buff[];
__at (ROAD_SCREEN_BUFFER_START) char screen_road_buf[];
__at (DASH_SCREEN_BUFFER_START) char screen_dash_buf[];
__sfr __at 0xfe joystickKeysPort;

enum {
  G_BUF,
  G_BG_SHIFT,
  G_TRACK_POS,
  G_TURN,
  G_SPEED,
  NUM_GLOBALS
};

#define A_BUF          0
#define A_BG_SHIFT     2
#define A_TRACK_POS    4
#define A_TURN         6
#define A_SPEED        8

static int globals[NUM_GLOBALS];

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

int shifts[64];
const int turns[19] = {32, 45, 64, 90, 128, 181, 256, 362, 512, 0, -512, -362, -256, -181, -128, -90, -64, -45, -32};
unsigned char double_buf[0x800];
int squares[64];

unsigned int track_position;

void render_road();
void mark(int pos, unsigned char line, unsigned char *pp);
void calc_shifts(int factor);
void render_background();
void render_dashboard();
void render_map();
void render_pos(unsigned int pos);
void plot(unsigned char x, unsigned char y);
void init();

int main() {
  int turn;

  init();
  render_dashboard();

  while (1) {
    globals[G_SPEED] = 1;
    turn = track[(track_position / 64) + AHEAD].t;
    render_pos((track_position / 64));
    calc_shifts(turns[turn + 9]);
    globals[G_TURN] = turn;
    render_background();
    render_road();
    render_pos(track_position / 64);

    track_position += globals[G_SPEED];
    if ((track_position / 64) >= TRACK_SIZE) {
      track_position = 0;
    }

  }
  return 0;
}

void init() {
  unsigned char i;
  track_position = 0;
  for (i=0;i<64;i++) squares[i] = i*i;
}

void calc_shifts(int factor) {
  char i;
  for (i=0; i<64; i++) {
    shifts[63 - i] = squares[i] / factor;
  }
}

void mark(int pos, unsigned char line, unsigned char *pp) {
  unsigned char fill_buf;
  unsigned int x;
  pos += 127;
  if (pos < 0x00) return;
  x = pos + shifts[line];
  if (x > 0xff) return;
  fill_buf = 0x80 >> (x & 0x07);
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
      if (road_marks[m].solid || (((line / 8) & 0x03) == ((track_position / 8) & 0x03))) {
        w = line * road_marks[m].angle;
        mark(+road_marks[m].width + w, line, p);
        if (road_marks[m].width == 0) break;
        mark(-road_marks[m].width - w, line, p);
      }
    }
  }
  memcpy(screen_road_buf, double_buf, 0x800);
}

void render_dashboard() {
  memcpy(screen_dash_buf, bin2c_dashboard_scr + 0x1000, 0x800);
  render_map();
}

void render_background(){
  __asm
  ld iy, #_globals

  ld e, A_TURN(iy)
  xor a
  bit 7, e                       ; Expand signed char to int
  jr z, expand_char
  cpl
expand_char:
  ld d, a
  ld a, A_SPEED(iy)
  call #_mult12                  ; Multiply speed by turn angle
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

  ld A_BUF(iy), a                ; Store image shift in a buffer

  ld c, a
  ld b, #0
  ld de, #_screen_buff           ; Load screen buffer address
  ld hl, #_bin2c_dashboard_scr   ; Load background start address
  add hl, bc                     ; Shift image

  ld b, #0x40                    ; Load lines number
render_background_loop:          ; Loop by lines

  push bc                        ; Store lines loop counter

  ld b, #0
  ld a, #0x20
  sbc a, A_BUF(iy)               ; Calc number of bytes in first part
  jp z, skip_ldir_1              ; Do not copy zero bytes
  ld c, a
  ldir                           ; Copy first part
skip_ldir_1:

  ld bc, #0x0020
  sbc hl, bc
  ld b, #0
  xor a
  add a, A_BUF(iy)          ; Calc number of bytes in second part
  jp z, skip_ldir_2              ; Do not copy zero bytes
  ld c, a
  ldir                           ; Copy second part
skip_ldir_2:

  ld bc, #0x0020                 ; Shift image to next line
  add hl, bc

  pop bc                         ; Restore lines loop counter
  djnz render_background_loop
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
  plot(168 + x, 0 + y);
}

void render_map() {
  unsigned char i;
  for (i=0;i<TRACK_SIZE;i++) render_pos(i);
}

void mult12() {
  __asm
  ld hl,#0                        ; HL is used to accumulate the result
  ld b,#8                         ; the multiplier (A) is 8 bits wide
Mul8Loop:
  rrca                           ; putting the next bit into the carry
  jp nc,Mul8Skip                 ; if zero, we skip the addition (jp is used for speed)
  add hl,de                      ; adding to the product if necessary
Mul8Skip:
  sla e                          ; calculating the next auxiliary product by shifting
  rl d                           ; DE one bit leftwards (refer to the shift instructions!)
  djnz Mul8Loop
  __endasm;
}