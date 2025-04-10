
#include <string.h>

typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short word;

volatile __sfr __at (0x0) input0;
volatile __sfr __at (0x1) input1;
volatile __sfr __at (0x2) input2;
__sfr __at (0x2) bitshift_offset;
volatile __sfr __at (0x3) bitshift_read;
__sfr __at (0x4) bitshift_value;
__sfr __at (0x6) watchdog_strobe;

byte __at (0x2400) vidmem[224][32]; // 256x224x1 video memory

#define FIRE1 (input1 & 0x10)
#define LEFT1 (input1 & 0x20)
#define RIGHT1 (input1 & 0x40)
#define COIN1 (input1 & 0x1)
#define START1 (input1 & 0x4)
#define START2 (input1 & 0x2)

void scanline96() __interrupt;
void scanline224() __interrupt;

void main();
// start routine @ 0x0
// set stack pointer, enable interrupts
void start() __naked {
__asm
	LD      SP,#0x2400
        EI
        NOP
  	JP	_main
__endasm;
}

// scanline 96 interrupt @ 0x8
// we don't have enough bytes to make this an interrupt
// because the next routine is at 0x10
void _RST_8() __naked {
__asm
	NOP
        NOP
        NOP
        NOP
        NOP
  	JP	_scanline96
__endasm;
}

// scanline 224 interrupt @ 0x10
// this one, we make an interrupt so it saves regs.
void scanline224() __interrupt {
  vidmem[2]++;
}

// scanline 96 function, saves regs
void scanline96() __interrupt {
  vidmem[0]++;
}

/// GRAPHICS FUNCTIONS

void clrscr() {
  memset(vidmem, 0, sizeof(vidmem));
}

inline void xor_pixel(byte x, byte y) {
  byte* dest = &vidmem[x][y>>3];
  *dest ^= 0x1 << (y&7);
}

void draw_vline(byte x, byte y1, byte y2) {
  byte yb1 = y1/8;
  byte yb2 = y2/8;
  byte* dest = &vidmem[x][yb1];
  signed char nbytes = yb2 - yb1;
  *dest++ ^= 0xff << (y1&7);
  if (nbytes > 0) {
    while (--nbytes > 0) {
      *dest++ ^= 0xff;
    }
    *dest ^= 0xff >> (~y2&7);
  } else {
    *--dest ^= 0xff << ((y2+1)&7);
  }
}

#define LOCHAR 0x20
#define HICHAR 0x5e

const byte font8x8[HICHAR-LOCHAR+1][8] = {/*{w:8,h:8,bpp:1,count:63,xform:"rotate(-90deg)"}*/
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, { 0x00,0x00,0x00,0x79,0x79,0x00,0x00,0x00 }, { 0x00,0x70,0x70,0x00,0x00,0x70,0x70,0x00 }, { 0x14,0x7f,0x7f,0x14,0x14,0x7f,0x7f,0x14 }, { 0x00,0x12,0x3a,0x6b,0x6b,0x2e,0x24,0x00 }, { 0x00,0x63,0x66,0x0c,0x18,0x33,0x63,0x00 }, { 0x00,0x26,0x7f,0x59,0x59,0x77,0x27,0x05 }, { 0x00,0x00,0x00,0x10,0x30,0x60,0x40,0x00 }, { 0x00,0x00,0x1c,0x3e,0x63,0x41,0x00,0x00 }, { 0x00,0x00,0x41,0x63,0x3e,0x1c,0x00,0x00 }, { 0x08,0x2a,0x3e,0x1c,0x1c,0x3e,0x2a,0x08 }, { 0x00,0x08,0x08,0x3e,0x3e,0x08,0x08,0x00 }, { 0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x00 }, { 0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00 }, { 0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x00 }, { 0x00,0x01,0x03,0x06,0x0c,0x18,0x30,0x20 }, { 0x00,0x3e,0x7f,0x49,0x51,0x7f,0x3e,0x00 }, { 0x00,0x01,0x11,0x7f,0x7f,0x01,0x01,0x00 }, { 0x00,0x23,0x67,0x45,0x49,0x79,0x31,0x00 }, { 0x00,0x22,0x63,0x49,0x49,0x7f,0x36,0x00 }, { 0x00,0x0c,0x0c,0x14,0x34,0x7f,0x7f,0x04 }, { 0x00,0x72,0x73,0x51,0x51,0x5f,0x4e,0x00 }, { 0x00,0x3e,0x7f,0x49,0x49,0x6f,0x26,0x00 }, { 0x00,0x60,0x60,0x4f,0x5f,0x70,0x60,0x00 }, { 0x00,0x36,0x7f,0x49,0x49,0x7f,0x36,0x00 }, { 0x00,0x32,0x7b,0x49,0x49,0x7f,0x3e,0x00 }, { 0x00,0x00,0x00,0x12,0x12,0x00,0x00,0x00 }, { 0x00,0x00,0x00,0x13,0x13,0x00,0x00,0x00 }, { 0x00,0x08,0x1c,0x36,0x63,0x41,0x41,0x00 }, { 0x00,0x14,0x14,0x14,0x14,0x14,0x14,0x00 }, { 0x00,0x41,0x41,0x63,0x36,0x1c,0x08,0x00 }, { 0x00,0x20,0x60,0x45,0x4d,0x78,0x30,0x00 }, { 0x00,0x3e,0x7f,0x41,0x59,0x79,0x3a,0x00 }, { 0x00,0x1f,0x3f,0x68,0x68,0x3f,0x1f,0x00 }, { 0x00,0x7f,0x7f,0x49,0x49,0x7f,0x36,0x00 }, { 0x00,0x3e,0x7f,0x41,0x41,0x63,0x22,0x00 }, { 0x00,0x7f,0x7f,0x41,0x63,0x3e,0x1c,0x00 }, { 0x00,0x7f,0x7f,0x49,0x49,0x41,0x41,0x00 }, { 0x00,0x7f,0x7f,0x48,0x48,0x40,0x40,0x00 }, { 0x00,0x3e,0x7f,0x41,0x49,0x6f,0x2e,0x00 }, { 0x00,0x7f,0x7f,0x08,0x08,0x7f,0x7f,0x00 }, { 0x00,0x00,0x41,0x7f,0x7f,0x41,0x00,0x00 }, { 0x00,0x02,0x03,0x41,0x7f,0x7e,0x40,0x00 }, { 0x00,0x7f,0x7f,0x1c,0x36,0x63,0x41,0x00 }, { 0x00,0x7f,0x7f,0x01,0x01,0x01,0x01,0x00 }, { 0x00,0x7f,0x7f,0x30,0x18,0x30,0x7f,0x7f }, { 0x00,0x7f,0x7f,0x38,0x1c,0x7f,0x7f,0x00 }, { 0x00,0x3e,0x7f,0x41,0x41,0x7f,0x3e,0x00 }, { 0x00,0x7f,0x7f,0x48,0x48,0x78,0x30,0x00 }, { 0x00,0x3c,0x7e,0x42,0x43,0x7f,0x3d,0x00 }, { 0x00,0x7f,0x7f,0x4c,0x4e,0x7b,0x31,0x00 }, { 0x00,0x32,0x7b,0x49,0x49,0x6f,0x26,0x00 }, { 0x00,0x40,0x40,0x7f,0x7f,0x40,0x40,0x00 }, { 0x00,0x7e,0x7f,0x01,0x01,0x7f,0x7e,0x00 }, { 0x00,0x7c,0x7e,0x03,0x03,0x7e,0x7c,0x00 }, { 0x00,0x7f,0x7f,0x06,0x0c,0x06,0x7f,0x7f }, { 0x00,0x63,0x77,0x1c,0x1c,0x77,0x63,0x00 }, { 0x00,0x70,0x78,0x0f,0x0f,0x78,0x70,0x00 }, { 0x00,0x43,0x47,0x4d,0x59,0x71,0x61,0x00 }, { 0x00,0x00,0x7f,0x7f,0x41,0x41,0x00,0x00 }, { 0x00,0x20,0x30,0x18,0x0c,0x06,0x03,0x01 }, { 0x00,0x00,0x41,0x41,0x7f,0x7f,0x00,0x00 }, { 0x00,0x08,0x18,0x3f,0x3f,0x18,0x08,0x00 }
};

// byte __at (0x2400) vidmem[224][32]; // 256x224x1 video memory

void draw_sprite(const byte* src, byte x, byte y) {
  byte i,j;
  byte* dest = &vidmem[x][y];
  byte w = *src++;
  byte h = *src++;

  for (j=0; j<h; j++) {
    for (i=0; i<w; i++) {
      *dest++ = *src++;
    }
    
    dest += 32-w;
  }
}

byte xor_sprite(const byte* src, byte x, byte y) {
  byte i,j;
  byte result = 0;
  byte* dest = &vidmem[x][y];
  byte w = *src++;
  byte h = *src++;
  for (j=0; j<h; j++) {
    for (i=0; i<w; i++) {
      result |= (*dest++ ^= *src++);
    }
    dest += 32-w;
  }
  return result;
}

void erase_sprite(const byte* src, byte x, byte y) {
  byte i,j;
  byte* dest = &vidmem[x][y];
  byte w = *src++;
  byte h = *src++;
  for (j=0; j<h; j++) {
    for (i=0; i<w; i++) {
      *dest++ &= ~(*src++);
    }
    dest += 32-w;
  }
}

void clear_sprite(const byte* src, byte x, byte y) {
  byte i,j;
  byte* dest = &vidmem[x][y];
  byte w = *src++;
  byte h = *src++;
  for (j=0; j<h; j++) {
    for (i=0; i<w; i++) {
      *dest++ = 0;
    }
    dest += 32-w;
  }
}

void draw_char(char ch, byte x, byte y) {
  byte i;
  const byte* src = &font8x8[(ch-LOCHAR)][0];
  byte* dest = &vidmem[x*8][y];
  for (i=0; i<8; i++) {
    *dest = *src;
    dest += 32;
    src += 1;
  }
}

void draw_string(const char* str, byte x, byte y) {
  do {
    byte ch = *str++;
    if (!ch) break;
    draw_char(ch, x, y);
    x++;
  } while (1);
}

void draw_bcd_word(word bcd, byte x, byte y) {
  byte j;
  x += 3;
  for (j=0; j<4; j++) {
    draw_char('0'+(bcd&0xf), x, y);
    x--;
    bcd >>= 4;
  }
}

// add two 16-bit BCD values
word bcd_add(word a, word b) __naked {
  a; b; // to avoid warning
__asm
 	push	ix
 	ld	ix,#0
	add	ix,sp
 	ld	a,4 (ix)
 	add	a, 6 (ix)
	daa
	ld	c,a
 	ld	a,5 (ix)
 	adc	a, 7 (ix)
	daa
 	ld	b,a
 	ld	l, c
 	ld	h, b
	pop	ix
 	ret
__endasm;
}

//
// GAME GRAPHICS
//

const byte player_bitmap[] =
{2,27,/*{w:16,h:27,bpp:1,xform:"rotate(-90deg)"}*/0x0,0x0,0x0,0x0,0x0f,0x00,0x3e,0x00,0xf4,0x07,0xec,0x00,0x76,0x00,0x2b,0x00,0x33,0x00,0x75,0x00,0xf5,0x00,0xeb,0x31,0xbf,0xef,0x3f,0xcf,0xbf,0xef,0xeb,0x31,0xf5,0x00,0x75,0x00,0x33,0x00,0x2b,0x00,0x76,0x00,0xec,0x00,0xf4,0x07,0x3e,0x00,0x0f,0x00,0x00,0x00,0x0,0x0};
const byte bomb_bitmap[] =
{1,5,/*{w:8,h:5,bpp:1,xform:"rotate(-90deg)"}*/0x88,0x55,0x77,0x55,0x88};
const byte bullet_bitmap[] =
{2,2,/*{w:16,h:2,bpp:1,xform:"rotate(-90deg)"}*/0x88,0x88,0x44,0x44};
const byte enemy1_bitmap[] =
{2,17,/*{w:16,h:17,bpp:1,xform:"rotate(-90deg)"}*/0x00,0x00,0x00,0x0c,0x04,0x1e,0x46,0x3f,0xb8,0x7f,0xb0,0x7f,0xba,0x7f,0xfd,0x3f,0xfc,0x07,0xfc,0x07,0xfd,0x3f,0xba,0x7f,0xb0,0x7f,0xb8,0x7f,0x46,0x3f,0x04,0x1e,0x00,0x0c};
const byte enemy2_bitmap[] =
{2,16,/*{w:16,h:16,bpp:1,xform:"rotate(-90deg)"}*/0x26,0x00,0x59,0x10,0x10,0x30,0x33,0x18,0xe6,0x61,0xc4,0x56,0x03,0x03,0xdc,0x03,0xdc,0x03,0x03,0x03,0xc4,0x56,0xe6,0x61,0x33,0x18,0x10,0x30,0x59,0x10,0x26,0x00};
const byte enemy3_bitmap[] =
{2,16,/*{w:16,h:16,bpp:1,xform:"rotate(-90deg)"}*/0x80,0x1f,0xc0,0x03,0xf8,0x3f,0x70,0x00,0xf0,0x01,0xfc,0x07,0xe8,0x01,0xf8,0x03,0xf8,0x03,0xe8,0x01,0xf8,0x07,0xf0,0x01,0x70,0x00,0xf8,0x3f,0xc0,0x03,0x80,0x1f};
const byte enemy4_bitmap[] =
{2,16,/*{w:16,h:16,bpp:1,xform:"rotate(-90deg)"}*/0x06,0x00,0x0c,0x00,0x28,0x00,0x70,0x1f,0x84,0x3f,0xde,0x37,0xbb,0x3f,0xf0,0x3f,0xf0,0x3f,0xbb,0x3f,0xde,0x37,0x84,0x3f,0x70,0x1f,0x28,0x00,0x0c,0x00,0x06,0x00};

const byte* const enemy_bitmaps[4] = {
  enemy1_bitmap,
  enemy2_bitmap,
  enemy3_bitmap,
  enemy4_bitmap
};

//
// GAME CODE
//

byte attract;
byte credits;
byte curplayer;

word score;
byte lives;

#define MAXLIVES 5

byte player_x;
byte bullet_x;
byte bullet_y;
byte bomb_x;
byte bomb_y;

typedef struct {
  byte x,y;
  const byte* shape; // need const here
} Enemy;

#define MAX_ENEMIES 28

Enemy enemies[MAX_ENEMIES];
byte enemy_index;
byte num_enemies;

typedef struct {
  byte right:1;
  byte down:1;
} MarchMode;

MarchMode this_mode, next_mode;

void draw_lives(byte player) {
  byte i;
  byte n = lives;
  byte x = player ? (22-MAXLIVES) : 6;
  byte y = 30;
  for (i=0; i<MAXLIVES; i++) {
    draw_char(i<n?'*':' ', x++, y);
  }
}

void draw_score(byte player) {
  byte x = player ? 24 : 0;
  byte y = 30;
  draw_bcd_word(score, x, y);
}

void add_score(word pts) {
  if (attract) return;
  score = bcd_add(score, pts);
  draw_score(curplayer);
}

void xor_player_derez() {
  byte i,j;
  byte x = player_x+13;
  byte y = 8;
  byte* rand = 0;
  for (j=1; j<=0x1f; j++) {
    for (i=0; i<50; i++) {
      signed char xx = x + (*rand++ & 0x1f) - 15;
      signed char yy = y + (*rand++ & j);
      xor_pixel(xx, yy);
    }
  }
}

void destroy_player() {
  xor_player_derez(); // xor derez pattern
  xor_sprite(player_bitmap, player_x, 1); // erase ship via xor
  xor_player_derez(); // xor 2x to erase derez pattern
  player_x = 0xff;
  lives--;
}

void init_enemies() {
  byte i,x,y,bm;
  x=0;
  y=26;
  bm=0;
  for (i=0; i<MAX_ENEMIES; i++) {
    Enemy* e = &enemies[i];
    e->x = x;
    e->y = y;
    e->shape = enemy_bitmaps[bm];
    x += 28;
    if (x > 180) {
      x = 0;
      y -= 3;
      bm++;
    }
  }
  enemy_index = 0;
  num_enemies = MAX_ENEMIES;
  this_mode.right = 1;
  this_mode.down = 0;
  next_mode.right = 1;
  next_mode.down = 0;
}

void delete_enemy(Enemy* e) {
  clear_sprite(e->shape, e->x, e->y);
  memmove(e, e+1, sizeof(Enemy)*(enemies-e+MAX_ENEMIES-1));
  num_enemies--; // update_next_enemy() will check enemy_index
}

void update_next_enemy() {
  Enemy* e;
  if (enemy_index >= num_enemies) {
    enemy_index = 0;
    memcpy(&this_mode, &next_mode, sizeof(this_mode));
  }
  e = &enemies[enemy_index];
  clear_sprite(e->shape, e->x, e->y);
  if (this_mode.down) {
    // if too close to ground, end game
    if (--e->y < 5) {
      destroy_player();
      lives = 0;
    }
    next_mode.down = 0;
  } else {
    if (this_mode.right) {
      e->x += 2;
      if (e->x >= 200) {
        next_mode.down = 1;
        next_mode.right = 0;
      }
    } else {
      e->x -= 2;
      if (e->x == 0) {
        next_mode.down = 1;
        next_mode.right = 1;
      }
    }
  }
  draw_sprite(e->shape, e->x, e->y);
  enemy_index++;
}

void draw_bunker(byte x, byte y, byte y2, byte h, byte w) {
  byte i;
  for (i=0; i<h; i++) {
    draw_vline(x+i, y+i, y+y2+i*2);
    draw_vline(x+h*2+w-i-1, y+i, y+y2+i*2);
  }
  for (i=0; i<w; i++) {
    draw_vline(x+h+i, y+h, y+y2+h*2);
  }
}

void draw_playfield() {
  byte i;
  clrscr();
  draw_string("PLAYER 1", 0, 31);
  draw_score(0);
  draw_lives(0);
  //draw_string("PLAYER 2", 20, 31);
  //draw_score(1);
  //draw_lives(1);
  for (i=0; i<224; i++)
    vidmem[i][0] = 0x7f & 0x55;
  draw_bunker(30, 40, 15, 15, 20);
  draw_bunker(140, 40, 15, 15, 20);
}

char in_rect(Enemy* e, byte x, byte y, byte w, byte h) {
  byte eh = e->shape[0];
  byte ew = e->shape[1];
  return (x >= e->x-w && x <= e->x+ew && y >= e->y-h && y <= e->y+eh);
}

Enemy* find_enemy_at(byte x, byte y) {
  byte i;
  for (i=0; i<num_enemies; i++) {
    Enemy* e = &enemies[i];
    if (in_rect(e, x, y, 2, 0)) {
      return e;
    }
  }
  return NULL;
}

void check_bullet_hit(byte x, byte y) {
  Enemy* e = find_enemy_at(x,y);
  if (e) {
    delete_enemy(e);
    add_score(0x25);
  }
}

void fire_bullet() {
  bullet_x = player_x + 13;
  bullet_y = 3;
  xor_sprite(bullet_bitmap, bullet_x, bullet_y); // draw
}

void move_bullet() {
  byte leftover = xor_sprite(bullet_bitmap, bullet_x, bullet_y); // erase
  if (leftover || bullet_y > 26) {
    clear_sprite(bullet_bitmap, bullet_x, bullet_y);
    check_bullet_hit(bullet_x, bullet_y+2);
    bullet_y = 0;
  } else {
    bullet_y++;
    xor_sprite(bullet_bitmap, bullet_x, bullet_y); // draw
  }
}

void drop_bomb() {
  Enemy* e = &enemies[enemy_index];
  bomb_x = e->x + 7;
  bomb_y = e->y - 2;
  xor_sprite(bomb_bitmap, bomb_x, bomb_y);
}

void move_bomb() {
  byte leftover = xor_sprite(bomb_bitmap, bomb_x, bomb_y); // erase
  if (bomb_y < 2) {
    bomb_y = 0;
  } else if (leftover) {
    erase_sprite(bomb_bitmap, bomb_x, bomb_y); // erase bunker
    if (bomb_y < 3) {
      // player was hit (probably)
      destroy_player();
    }
    bomb_y = 0;
  } else {
    bomb_y--;
    xor_sprite(bomb_bitmap, bomb_x, bomb_y);
  }
}

byte frame;

void move_player() {
  if (attract) {
    if (bullet_y == 0) fire_bullet();
  } else {
    if (LEFT1 && player_x>0) player_x -= 2;
    if (RIGHT1 && player_x<198) player_x += 2;
    if (FIRE1 && bullet_y == 0) {
      fire_bullet();
    }
  }
  draw_sprite(player_bitmap, player_x, 1);
}

void play_round() {
  watchdog_strobe = 0;
  draw_playfield();
  player_x = 96;
  bullet_y = 0;
  bomb_y = 0;
  frame = 0;
  while (player_x != 0xff && num_enemies) {
    move_player();
    if (bullet_y) {
      move_bullet();
    }
    update_next_enemy();
    if (frame & 1) {
      if (bomb_y == 0) {
        drop_bomb();
      } else {
        move_bomb();
      }
    }
    watchdog_strobe = 0;
    frame++;
  }
}

void init_game() {
  score = 0;
  lives = 5;
  curplayer = 0;
}

void game_over_msg() {
  byte i;
  for (i=0; i<50; i++) {
    draw_string(" *************** ", 5, 15);
    draw_string("***           ***", 5, 16);
    draw_string("**  GAME OVER  **", 5, 17);
    draw_string("***           ***", 5, 18);
    draw_string(" *************** ", 5, 19);
    watchdog_strobe = 0;
  }
}

void play_game() {
  attract = 0;
  init_game();
  init_enemies();
  while (lives) {
    play_round();
    if (num_enemies == 0) {
      init_enemies();
    }
  }
  game_over_msg();
}

void attract_mode() {
  attract = 1;
  while (1) {
    init_enemies();
    play_round();
  }
}

void main() {
  // NOTE: initializers don't get run, so we init here
  credits = 0;
  while (1) {
    //attract_mode();
    play_game();
  }
}
