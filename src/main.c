#include <string.h>

#include "globals.h"

#include "dashboard.h"
#include "background1.h"
#include "mis_success.h"
#include "mis_fail.h"
#include "task.h"
#include "task_success.h"
#include "crash.h"
#include "success.h"
#include "fail.h"

#include "tracks.h"
#include "sprites.h"
#include "mapsprites.h"
#include "messages.h"
#include "font1.h"

#define SCREEN_BUFFER_START      0x4000
#define ATTR_SCREEN_BUFFER_START 0x5800
#define ROAD_SCREEN_BUFFER_START 0x4800
#define DASH_SCREEN_BUFFER_START 0x5000
#define DOUBLE_BUFFER_START      0x6000
#define SHIFTS_BUFFER_START      0x6800
#define GLOBALS_START            0x6880
#define SPRITE_BUFFER_START      0x6900
#define TRACK_BUFFER_START       0x6930

__at (SCREEN_BUFFER_START) char screen_buf[0x1800];
__at (ATTR_SCREEN_BUFFER_START) char screen_attr_buf[0x300];
__at (ROAD_SCREEN_BUFFER_START) char screen_road_buf[0x800];
__at (DASH_SCREEN_BUFFER_START) char screen_dash_buf[0x800];
__at (DOUBLE_BUFFER_START) char double_buf[0x800];
__at (SHIFTS_BUFFER_START) int shifts[0x80];
__at (GLOBALS_START) int globals[NUM_GLOBALS];
__at (SPRITE_BUFFER_START) char sprite_buf[0x30];
__at (TRACK_BUFFER_START) track_element_t track[200];


__sfr __at 0xfe joystickKeysPort;

void render_road();
void mark(int pos, unsigned char line, unsigned char *pp);
void calc_shifts();
void render_background();
void render_dashboard();
void render_pos();
void sprite_to_buf(int x, int y, char *sprite);
void render_buf(int x, int y, char *buf);
unsigned int random();
void cls();
void go(int new_state);
void printf(char *s, char x, char y);
void print_digit();
void print_number(char x, char y, int val);
void render_map(char *map);

int main() {
  globals[G_MISSION] = 0;
  globals[G_COARSE_POS] = 0;
  globals[G_TRACK_POS] = 0;
  globals[G_LIVES] = 5;

  go(ST_INTRO);

  while (1) {
    switch(globals[G_STATE]) {
      case ST_INTRO:
        if (FIRE) go(ST_MIS_INTRO);
        break;
      case ST_MIS_INTRO:
        if (FIRE) go(ST_RACE);
        break;
      case ST_RACE_END:
        if (FIRE) go(ST_QUEST);
        break;
      case ST_QUEST:
        if (LEFT || RIGHT || FORWARD) {
          switch (globals[G_MISSION]) {
            case 0: if (RIGHT_ANSWER1) go (ST_MIS_SUCCESS); else go (ST_MIS_FAIL); break;
            case 1: if (RIGHT_ANSWER2) go (ST_MIS_SUCCESS); else go (ST_MIS_FAIL); break;
            case 2: if (RIGHT_ANSWER3) go (ST_MIS_SUCCESS); else go (ST_MIS_FAIL); break;
            case 3: if (RIGHT_ANSWER4) go (ST_MIS_SUCCESS); else go (ST_MIS_FAIL); break;
            case 4: if (RIGHT_ANSWER5) go (ST_MIS_SUCCESS); else go (ST_MIS_FAIL); break;
            case 5: if (RIGHT_ANSWER6) go (ST_MIS_SUCCESS); else go (ST_MIS_FAIL); break;
            case 6: if (RIGHT_ANSWER7) go (ST_MIS_SUCCESS); else go (ST_MIS_FAIL); break;
            case 7: if (RIGHT_ANSWER8) go (ST_SUCCESS); else go (ST_MIS_FAIL); break;
          }
        }
        break;
      case ST_MIS_SUCCESS:
        if (FIRE) {
          globals[G_MISSION]++;
          if (globals[G_MISSION] >= NUM_MISSIONS) {
            go(ST_SUCCESS);
          } else {
            go(ST_MIS_INTRO);
          }
        }
        break;
      case ST_CRASH:
        if (globals[G_LIVES] <= 0) go (ST_FAIL);
        if (FIRE || FIRE) go(ST_RACE);
        break;
      case ST_MIS_FAIL:
        if (globals[G_LIVES] <= 0) go (ST_FAIL);
        if (FIRE || FIRE) go(ST_QUEST);
        break;
      case ST_RACE:
        if (BACK) globals[G_SPEED]--;
        if (FORWARD) globals[G_SPEED]++;
        if (RIGHT) globals[G_MY_ANGLE]-=2;
        if (LEFT) globals[G_MY_ANGLE]+=2;
        if (globals[G_SPEED] > MAX_SPEED) globals[G_SPEED] = MAX_SPEED;
        if (globals[G_SPEED] < 0) globals[G_SPEED] = 0;

        globals[G_TRACK_POS] += globals[G_SPEED];
        globals[G_COARSE_POS] = globals[G_TRACK_POS] >> 4;

        if (globals[G_COARSE_POS] > globals[G_FINISH]) {  // cycle track
          globals[G_COARSE_POS] = 0;
          globals[G_TRACK_POS] = 0;
        }

        if (globals[G_COARSE_POS] >= globals[G_FINISH])  { //end of race
          go(ST_RACE_END);
          break;
        };

        globals[G_TURN] = track[globals[G_COARSE_POS]].t;
        globals[G_ROAD_ANGLE] = track[globals[G_COARSE_POS]].t;
        globals[G_MISANGLE] = globals[G_MY_ANGLE] - globals[G_ROAD_ANGLE];
        globals[G_MISPOS] += ((globals[G_SPEED] * globals[G_MISANGLE]) >> 2);

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
        render_pos();
        render_road();
        render_pos();
        print_number(3, 17, globals[G_SPEED]);
        print_number(3, 18, globals[G_TIME]);
        print_number(3, 19, globals[G_LIVES]);

        if (globals[G_MISPOS] > 150 || globals[G_MISPOS] < -150) {
            go(ST_CRASH);
        }

        if (globals[G_TIMER] == 0) {
          globals[G_TIMER] = FPS;
          globals[G_TIME]--;
          if (globals[G_TIME] <= 0) go(ST_CRASH);
        } else {
          globals[G_TIMER]--;
        }
        break;
    }
  }
  return 0;
}

void go(int new_state) {
  switch (new_state) {
    case ST_RACE:
      globals[G_SPEED] = 0;
      globals[G_COARSE_POS] = 0;
      globals[G_TRACK_POS] = 0;
      globals[G_ROAD_ANGLE] = track[globals[G_COARSE_POS]].t;
      globals[G_MY_ANGLE] = globals[G_ROAD_ANGLE];
      globals[G_MISPOS] = 0;
      globals[G_OLD_BG_SHIFT] = 0xff;
      globals[G_SPRITE_POS] = 0;
      globals[G_SPRITE_Y] = 0;
      globals[G_TIMER] = 0;
      cls();
      render_dashboard();
      switch (globals[G_MISSION]) {
        case 0:
          globals[G_TIME] = TRACK1_TIME;
          globals[G_FINISH] = TRACK1_SIZE;
          memcpy(track, track1, TRACK1_SIZE * sizeof(track_element_t));
          render_map(map_sprites[0]);
          break;
        case 1:
          globals[G_TIME] = TRACK2_TIME;
          globals[G_FINISH] = TRACK2_SIZE;
          memcpy(track, track2, TRACK2_SIZE * sizeof(track_element_t));
          render_map(map_sprites[1]);
          break;
        case 2:
          globals[G_TIME] = TRACK3_TIME;
          globals[G_FINISH] = TRACK3_SIZE;
          memcpy(track, track3, TRACK3_SIZE * sizeof(track_element_t));
          render_map(map_sprites[2]);
          break;
        case 3:
          globals[G_TIME] = TRACK4_TIME;
          globals[G_FINISH] = TRACK4_SIZE;
          memcpy(track, track4, TRACK4_SIZE * sizeof(track_element_t));
          render_map(map_sprites[3]);
          break;
        case 4:
          globals[G_TIME] = TRACK5_TIME;
          globals[G_FINISH] = TRACK5_SIZE;
          memcpy(track, track5, TRACK5_SIZE * sizeof(track_element_t));
          render_map(map_sprites[4]);
          break;
        case 5:
          globals[G_TIME] = TRACK6_TIME;
          globals[G_FINISH] = TRACK6_SIZE;
          memcpy(track, track6, TRACK6_SIZE * sizeof(track_element_t));
          render_map(map_sprites[5]);
          break;
        case 6:
          globals[G_TIME] = TRACK7_TIME;
          globals[G_FINISH] = TRACK7_SIZE;
          memcpy(track, track7, TRACK7_SIZE * sizeof(track_element_t));
          render_map(map_sprites[6]);
          break;
        case 7:
          globals[G_TIME] = TRACK8_TIME;
          globals[G_FINISH] = TRACK8_SIZE;
          memcpy(track, track8, TRACK8_SIZE * sizeof(track_element_t));
          render_map(map_sprites[7]);
          break;
      }
      break;
    case ST_INTRO:
      cls();
      printf(msg_intro1, 0, 0);
      printf(msg_press_fire, 0, 23);
      break;
    case ST_MIS_INTRO:
      cls();
      memcpy(screen_dash_buf, bin2c_task_bin, 0x800);
      switch (globals[G_MISSION]) {
        case 0: printf(msg_mission1, 0, 3); break;
        case 1: printf(msg_mission2, 0, 3); break;
        case 2: printf(msg_mission3, 0, 3); break;
        case 3: printf(msg_mission4, 0, 3); break;
        case 4: printf(msg_mission5, 0, 3); break;
        case 5: printf(msg_mission6, 0, 3); break;
        case 6: printf(msg_mission7, 0, 3); break;
        case 7: printf(msg_mission8, 0, 3); break;
      }
      printf ("1-�����" EOL, 23, 18);
      printf ("2-������" EOL, 23, 19);
      printf ("3-����!" EOL, 23, 20);
      printf ("4-����!" EOL, 23, 21);
      printf(msg_press_fire, 0, 13);
      break;
    case ST_RACE_END:
      cls();
      memcpy(screen_dash_buf, bin2c_mis_success_bin, 0x800);
      printf(msg_mis_task1, 0, 3);
      printf(msg_press_fire, 0, 14);
      break;
    case ST_QUEST:
      cls();
      switch (globals[G_MISSION]) {
        case 0: printf(msg_quest1, 0, 0); break;
        case 1: printf(msg_quest2, 0, 0); break;
        case 2: printf(msg_quest3, 0, 0); break;
        case 3: printf(msg_quest4, 0, 0); break;
        case 4: printf(msg_quest5, 0, 0); break;
        case 5: printf(msg_quest6, 0, 0); break;
        case 6: printf(msg_quest7, 0, 0); break;
        case 7: printf(msg_quest8, 0, 0); break;
      }
      break;
    case ST_MIS_SUCCESS:
      cls();
      memcpy(screen_dash_buf, bin2c_task_success_bin, 0x800);
      printf(msg_task_success1, 0, 3);
      printf(msg_press_fire, 0, 14);
      break;
    case ST_SUCCESS:
      cls();
      memcpy(screen_dash_buf, bin2c_success_bin, 0x800);
      printf(msg_success1, 0, 0);
      break;
    case ST_CRASH:
      cls();
      globals[G_LIVES]--;
      if (globals[G_LIVES] > 0) {
        memcpy(screen_dash_buf, bin2c_crash_bin, 0x800);
        printf(msg_mis_fail1, 0, 0);
        switch (globals[G_LIVES]) {
          case 4: printf(msg_task_fail2, 0, 7); break;
          case 3: printf(msg_task_fail3, 0, 7); break;
          case 2: printf(msg_task_fail4, 0, 7); break;
          case 1: printf(msg_task_fail5, 0, 7); break;
        }
        printf(msg_mis_fail2, 0, 8);
        printf(msg_press_fire, 0, 13);
      }
      break;
    case ST_MIS_FAIL:
      cls();
      globals[G_LIVES]--;
      if (globals[G_LIVES] > 0) {
        memcpy(screen_dash_buf, bin2c_mis_fail_bin, 0x800);
        printf(msg_task_fail1, 0, 0);
        switch (globals[G_LIVES]) {
          case 4: printf(msg_task_fail2, 0, 3); break;
          case 3: printf(msg_task_fail3, 0, 3); break;
          case 2: printf(msg_task_fail4, 0, 3); break;
          case 1: printf(msg_task_fail5, 0, 3); break;
        }
        printf(msg_task_fail6, 0, 4);
        printf(msg_press_fire, 0, 13);
      }
      break;
    case ST_FAIL:
      cls();
      memcpy(screen_dash_buf, bin2c_fail_bin, 0x800);
      printf(msg_fail1, 0, 0);
      break;
  }
  globals[G_STATE] = new_state;
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
  sprite_to_buf(globals[G_SPRITE_X], globals[G_SPRITE_Y], globals[G_SPRITE_ADDR]);
  render_buf(globals[G_SPRITE_X], globals[G_SPRITE_Y], double_buf);
  memcpy(screen_road_buf, double_buf, 0x800);
}

void render_dashboard() {
  memcpy(screen_dash_buf, bin2c_dashboard_bin, 0x800);
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
  ex de, hl
  add hl, de
  add hl, de
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

void render_pos() {
  int pos = globals[G_COARSE_POS];
  int x = 80 + track[pos].x;
  int y = 55 - track[pos].y;
  if (x > 164) x = 164;
  if (y > 52) y = 52;
  sprite_to_buf(x, y, sprites[8]);
  render_buf(x, y, screen_dash_buf);
}

void sprite_to_buf(int x, int y, char *sprite) {
  __asm
  ld iy, #2
  add iy, sp

  ld e, 4(iy)
  ld d, 5(iy)
  ld hl, #_sprite_buf           ; Copy from sprite to buffer

  ld c, #16
  copy_sprite_loop:               ; Loop by lines of sprite

  inc hl                        ; HL to second byte of buffer

  xor a
  ld (hl), a                    ; Clean sprite buffer

  ld a, 0(iy)                   ; Load X pos
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

  ld a, 0(iy)                   ; Load X pos
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
  __endasm;
}

void render_buf(int x, int y, char *buf) {
  __asm
  ld iy, #2
  add iy, sp

  ld de, #_sprite_buf

  ld a, #64
  sub 2(iy)
  jr c, sprite_end

  ld c, #0
  ld b, #16
  cp b
  jr nc, render_sprite_inner_loop
  ld b, a
  render_sprite_inner_loop:

  ld l, 4(iy)
  ld h, 5(iy)

  ld a, 0(iy)
  srl a
  srl a
  srl a
  add a, l
  ld l, a

  ld a, c
  add 2(iy)
  and #0x07
  add a, h
  ld h, a

  ld a, c
  add 2(iy)
  add a, a
  add a, a
  and #0xe0
  add a, l
  ld l, a

  ld a, h
  adc a, #0
  ld h, a

  ld a, 1(iy)
  or a
  jr nz, skip1
  ld a, (de)
  xor (hl)
  ld (hl), a
  skip1:
  inc hl
  inc de

  ld a, 1(iy)
  or a
  jr nz, skip2
  ld a, 0(iy)
  cp #0xf8
  jr nc, skip2
  ld a, (de)
  xor (hl)
  ld (hl), a
  skip2:
  inc hl
  inc de

  ld a, 1(iy)
  or a
  jr nz, skip3
  ld a, 0(iy)
  cp #0xf0
  jr nc, skip3
  ld a, (de)
  xor (hl)
  ld (hl), a
  skip3:
  inc de

  inc c
  djnz render_sprite_inner_loop
  sprite_end:
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

void cls() {
  memset(screen_buf, 0x00, 0x1800);
  memset(screen_attr_buf, 0x07, 0x300);
}

/**
 * E - X
 * D - Y
 * @return DE - screen address
 */
void get_char_address() {
  __asm
  ld a,d            ; get screen address
  and #0x7
  rra
  rra
  rra
  rra
  or e
  ld e,a
  ld a,d
  and #0x18
  or #0x40
  ld d,a
  __endasm;
}

/**
 * A - char code
 * DE - screen addr
 * BC - charset addr
 */
void print_char() {
  __asm
  sbc hl, hl       ; multiply char code to 8
  ld l, a
  adc hl, hl
  adc hl, hl
  adc hl, hl
  add hl, bc       ; add charset address
  ld b, #8
print_char_loop:   ; copy character to screen
  ld a, (hl)
  ld (de), a
  inc hl
  inc d
  djnz print_char_loop
  __endasm;
}

/**
 * A - number (0-9)
 * DE - Screen addr
 */
void print_digit() {
  __asm
  ld bc, #0x3c00
  add a, #'0'
  jr #_print_char
  __endasm;
}

void print_number(char x, char y, int val) {
  __asm

  ld iy, #2               ; load arguments
  add iy, sp
  ld e, 0(iy)
  ld d, 1(iy)
  call #_get_char_address ; taints A
  ld a, 2(iy)

  ld bc, #0xff0a
bcd_loop:                 ; div 10
  inc b
  sub c
  jr nc, bcd_loop
  add a, c                ; B - first digit, A - last digit

  push de                 ; store coords
  push bc                 ; store first digit
  inc e                   ; print last digit first
  call #_print_digit      ; taints BC and DE
  pop af                  ; load stored first digit to A
  pop de
  jr #_print_digit        ; print first digit
  __endasm;
}

void printf(char *s, char x, char y) {
  __asm

  pop af             ; load arguments
  pop iy
  pop de
  push de
  push iy
  push af             ; pointer to string in IY here
  dec iy

print_loop:
  inc iy              ; next character
  ld a, (iy)          ; load next character
  cp #0x02            ; check for EOL
  ret z
  cp #0x0a            ; carriage return
  jr z, print_cr
  cp #0xc0            ; check for cp1251 symbol
  jr nc, print_1251
  ld bc, #0x3c00      ; load standard charset
  jr print_putc
print_1251:
  ld bc, #_font1 - 0x200 ; load cyrillic charset
  sub #0x80
print_putc:            ; charset addr in BC, character in A, pointer to string in IY

  halt                 ; delay
  push de              ; save coords
  ld l, a
  call #_get_char_address  ; taints A
  ld a, l
  call #_print_char
  pop de               ; restore coords

  inc e               ; increase X coord
  ld a, #0x1f         ; check against screen width
  cp e
  jr nc, print_loop
print_cr:
  xor a
  ld e, a             ; X coord = 0
  inc d               ; increase Y coord
  jr print_loop
  __endasm;
}

void render_map(char *map) {
  __asm
  pop af
  pop hl
  push hl
  push af

  ld b, #0x30
  ld de, #0x502a
map_loop:
  push bc
  ld bc, #0x0b
  ldir
  pop bc
  inc d

  ld a, #0xe0
  and e
  add #0x0a
  ld e, a

  ld a, #0x07
  and d
  jr nz, map_line
  push hl
  ex de, hl
  ld de, #0x07e0
  sbc hl, de
  ex de, hl
  pop hl

map_line:
  djnz map_loop
  __endasm;
}