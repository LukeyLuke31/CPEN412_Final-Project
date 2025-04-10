
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define PIXEL_BUFFER_VGA_BASEADDRESS   (0x01000000)
#define VIDMEM_DIM1_W_BLANK_EDGES (256)
#define VIDMEM_DIM1 (224)
#define VIDMEM_DIM2 (32)

#define VIDEOMEM_ADDR(x,y) ((volatile unsigned char *)(PIXEL_BUFFER_VGA_BASEADDRESS + ((y)*VIDMEM_DIM1_W_BLANK_EDGES)+(x)))
#define VIDMEM(x,y) (*VIDEOMEM_ADDR(x,y))
#define COSMIC_IMPALAS_TIMER_DELAY_MS (50)

#define WRITE_VIDMEM(x,y,CH) ((*(volatile unsigned char *)(PIXEL_BUFFER_VGA_BASEADDRESS + ((y)*VIDMEM_DIM1_W_BLANK_EDGES)+(x))) = CH)
#define MAX_ENEMIES 28

int FIRE1  ;
int LEFT1  ;
int RIGHT1 ;


#define LOCHAR 0x20
#define HICHAR 0x5e

//
// GAME CODE
//

#define MAXLIVES 5
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short word;

typedef struct {
  byte x;
  byte y;
  byte shape; 
} Enemy;



typedef struct {
  byte right;
  byte down;
} MarchMode;

MarchMode this_mode, next_mode;
byte enemy_index;
byte num_enemies;
byte player_x;
byte bullet_x;
byte bullet_y;
byte bomb_x;
byte bomb_y;

byte attract;
byte credits;
byte curplayer;

word score;
byte lives;
Enemy enemies[MAX_ENEMIES];
char font8x8[HICHAR-LOCHAR+1][8];

byte player_bitmap[56];
byte bomb_bitmap[7];
byte bullet_bitmap[6];
byte enemy1_bitmap[34];
byte enemy2_bitmap[34];
byte enemy3_bitmap[34];
byte enemy4_bitmap[34];
byte* enemy_bitmaps[4];
 
unsigned long seed; 
 
extern int clock_count_ms;
 
 
///////////////////////////////////////////////////////////////////////////
//
// Functions to Implement
//
///////////////////////////////////////////////////////////////////////////
void draw_sprite(byte* src, byte x, byte y)
{
 //complete this function
}

byte xor_sprite(byte *src, byte x, byte y)
{
//complete this function
}

void erase_sprite(byte *src, byte x, byte y)
{
  //complete this function
}

void clear_sprite(byte *src, byte x, byte y)
{
 //complete this function
}

void move_player() {
//complete this function
}

///////////////////////////////////////////////////////////////////////////
//
// End Functions to Implement
//
///////////////////////////////////////////////////////////////////////////


// Set the seed
void srand(unsigned long new_seed) {
    seed = new_seed;
}

// Generate a pseudorandom number
unsigned long long_rand(void) {
    seed ^= seed << 13; // XOR with shifted value
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

int clock() {
   return clock_count_ms;
}

void delay_ms(int num_ms) {
	int start_time;
	int current_time;
	start_time = clock();

	do {
	    current_time = clock();
	    if (current_time < start_time) { //handle wraparound
		   num_ms = num_ms - (INT_MAX-start_time);
		   start_time = current_time;
	    }
	} while ((current_time - start_time) < num_ms);
}

void clrscr() {
  int a;
  int b;
  for (a = 0; a < VIDMEM_DIM1; a++) {
    for (b = 0; b < VIDMEM_DIM2; b++) {
       WRITE_VIDMEM(a,b,0 /*0xff*/);	   
    }
  }

  //memset(vidmem, 0, VIDMEM_DIM1*VIDMEM_DIM2);
}

void xor_pixel(unsigned char x, unsigned char y) {
  unsigned char* dest;
  dest =  VIDEOMEM_ADDR(x,y>>3);
  *dest = (*dest) ^ (0x1 << (y&7));
}


void draw_vline(unsigned char x, unsigned char y1, unsigned char y2) {
  unsigned char yb1;
  unsigned char yb2;
  unsigned char val;
  unsigned char* dest;
  int nchars;
  yb1 = y1>>3;
  yb2 = y2>>3;
  nchars  = (((unsigned int) yb2) & 0xff) - (((unsigned int) yb1) & 0xff);
  dest = VIDEOMEM_ADDR(x,yb1);
  val = 0xff << (y1&7);
  *dest ^= val;
  dest = dest+VIDMEM_DIM1_W_BLANK_EDGES;
  if (nchars > 0) {
    while (--nchars > 0) {
	  val =  0xff;
      *dest ^= val;
      dest = dest+VIDMEM_DIM1_W_BLANK_EDGES;	  
    }
	val = (0xff >> (~y2&7));
    *dest ^= val;
  } else {
	dest = dest-VIDMEM_DIM1_W_BLANK_EDGES; 
	val = (0xff << ((y2+1)&7));
    *dest ^= val;
  }
}



void draw_char(unsigned char ch, unsigned char x, unsigned char y) {
  unsigned char i;
  unsigned char* src;
  unsigned char* dest;
  src  = &font8x8[(ch-LOCHAR)][0];
  dest = VIDEOMEM_ADDR(x*8,y);
  
  for (i=0; i<8; i++) {
    *dest = *src;
    dest += 1;//32;
    src += 1;
  }
}

void draw_string(char* str, byte x, byte y) {
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

// Function to add two BCD numbers
word bcd_add(word a, word b)
{
    word result, carry, place, digit_a, digit_b, sum;

    result = 0;  /* Final BCD result */
    carry = 0;   /* Carry for BCD addition */
    place = 0;   /* Bit position for reconstructing the result */

    while (a > 0 || b > 0 || carry > 0) {
        /* Extract the lowest BCD digit from each number using a mask */
        digit_a = a & 0xF;
        digit_b = b & 0xF;

        /* Add the digits and the carry */
        sum = digit_a + digit_b + carry;

        /* Perform BCD correction if sum > 9 */
        if (sum > 9) {
            sum -= 10;  /* Correct the sum */
            carry = 1;  /* Set the carry */
        } else {
            carry = 0;  /* Reset carry */
        }

        /* Add the corrected digit to the result in its proper place */
        result |= (sum << place);

        /* Move to the next higher BCD digit */
        a >>= 4;
        b >>= 4;
        place += 4;  /* Move to the next nibble */
    }

    return result;
}


void draw_lives(byte player) 
{
    byte i, n, x, y;

    n = lives;
    x = player ? (22 - MAXLIVES) : 6;
    y = 30;

    for (i = 0; i < MAXLIVES; i++) {
        draw_char(i < n ? '*' : ' ', x++, y);
    }
}

void draw_score(byte player) {
    byte x, y;

    x = player ? 24 : 0;
    y = 30;

    draw_bcd_word(score, x, y);
}

void add_score(word pts) {
  if (attract) return;
  score = bcd_add(score, pts);
  draw_score(curplayer);
}

byte get_char_rand() {
	byte rand;
    rand = (byte) (long_rand() & 0xff);  	
	return rand;
}

void xor_player_derez() {
    byte i, j, x, y, rand;
    signed char xx, yy;

    x = player_x + 13;
    y = 8;
    rand = get_char_rand();

    for (j = 1; j <= 0x1f; j++) {
        for (i = 0; i < 50; i++) {
			rand = get_char_rand();
            xx = x + (rand & 0x1f) - 15;
 		    rand = get_char_rand();
            yy = y + (rand & j);
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
    byte i, x, y, bm;

    x = 0;
    y = 26;
    bm = 0;

    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].x = x;
        enemies[i].y = y;
        enemies[i].shape = bm;
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
  clear_sprite(enemy_bitmaps[e->shape], e->x, e->y);
  memmove(e, e+1, sizeof(Enemy)*(enemies-e+MAX_ENEMIES-1));
  num_enemies--; // update_next_enemy() will check enemy_index
}

void update_next_enemy() {
  if (enemy_index >= num_enemies) {
    enemy_index = 0;
	this_mode.down = next_mode.down;
	this_mode.right = next_mode.right;
  }
  clear_sprite(enemy_bitmaps[enemies[enemy_index].shape], enemies[enemy_index].x, enemies[enemy_index].y);
  if (this_mode.down) {
    // if too close to ground, end game
	enemies[enemy_index].y = enemies[enemy_index].y-1;
    if (enemies[enemy_index].y < 5) {
      destroy_player();
      lives = 0;
    }
    next_mode.down = 0;
  } else {
    if (this_mode.right) {
      enemies[enemy_index].x += 2;
      if (enemies[enemy_index].x >= 200) {
        next_mode.down = 1;
        next_mode.right = 0;
      }
    } else {
      enemies[enemy_index].x -= 2;
      if (enemies[enemy_index].x == 0) {
        next_mode.down = 1;
        next_mode.right = 1;
      }
    }
  }
  draw_sprite(enemy_bitmaps[enemies[enemy_index].shape], enemies[enemy_index].x, enemies[enemy_index].y);
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
  for (i=0; i<224; i++) {
    WRITE_VIDMEM(i,0,0x7f & 0x55);
  }
  draw_bunker(30, 40, 15, 15, 20);
  draw_bunker(140, 40, 15, 15, 20);
}

char in_rect(e, x, y, w, h)
Enemy *e;
byte x, y, w, h;
{
    byte eh, ew;

    eh = enemy_bitmaps[e->shape][0];
    ew = enemy_bitmaps[e->shape][1];

    return (x >= e->x - w && x <= e->x + ew && y >= e->y - h && y <= e->y + eh);
}

Enemy *find_enemy_at(x, y)
byte x, y;
{
    byte i;
    Enemy *e;

    for (i = 0; i < num_enemies; i++) {
        e = &enemies[i];
        if (in_rect(e, x, y, 2, 0)) {
            return e;
        }
    }

    return NULL;
}

void check_bullet_hit(x, y)
byte x, y;
{
    Enemy *e;

    e = find_enemy_at(x, y);
    if (e) {
        delete_enemy(e);
        add_score(0x25);
    }
}

void fire_bullet()
{
    bullet_x = player_x + 13;
    bullet_y = 3;
    xor_sprite(bullet_bitmap, bullet_x, bullet_y); /* Draw bullet */
}

void move_bullet()
{
    byte leftover;

    leftover = xor_sprite(bullet_bitmap, bullet_x, bullet_y); /* Erase bullet */
    if (leftover || bullet_y > 26) {
        clear_sprite(bullet_bitmap, bullet_x, bullet_y);
        check_bullet_hit(bullet_x, bullet_y + 2);
        bullet_y = 0;
    } else {
        bullet_y++;
        xor_sprite(bullet_bitmap, bullet_x, bullet_y); /* Draw bullet */
    }
}

void drop_bomb()
{
    Enemy *e;

    e = &enemies[enemy_index];
    bomb_x = e->x + 7;
    bomb_y = e->y - 2;
    xor_sprite(bomb_bitmap, bomb_x, bomb_y); /* Draw bomb */
}

void move_bomb()
{
    byte leftover;

    leftover = xor_sprite(bomb_bitmap, bomb_x, bomb_y); /* Erase bomb */
    if (bomb_y < 2) {
        bomb_y = 0;
    } else if (leftover) {
        erase_sprite(bomb_bitmap, bomb_x, bomb_y); /* Erase bunker */
        if (bomb_y < 3) {
            /* Player was hit (probably) */
            destroy_player();
        }
        bomb_y = 0;
    } else {
        bomb_y--;
        xor_sprite(bomb_bitmap, bomb_x, bomb_y); /* Draw bomb */
    }
}


byte frame;


void play_round() {
  draw_playfield();
  player_x = 96;
  bullet_y = 0;
  bomb_y = 0;
  frame = 0;
  while (player_x != 0xff && num_enemies) {
	delay_ms(COSMIC_IMPALAS_TIMER_DELAY_MS);
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
    frame++;
  }
}

void init_game() {
  score = 0;
  lives = MAXLIVES;
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

void initialize_all_bitmaps() {

/* Initialize player_bitmap */
player_bitmap[0] = 2;
player_bitmap[1] = 27;
player_bitmap[2] = 0x00;
player_bitmap[3] = 0x00;
player_bitmap[4] = 0x00;
player_bitmap[5] = 0x00;
player_bitmap[6] = 0x0F;
player_bitmap[7] = 0x00;
player_bitmap[8] = 0x3E;
player_bitmap[9] = 0x00;
player_bitmap[10] = 0xF4;
player_bitmap[11] = 0x07;
player_bitmap[12] = 0xEC;
player_bitmap[13] = 0x00;
player_bitmap[14] = 0x76;
player_bitmap[15] = 0x00;
player_bitmap[16] = 0x2B;
player_bitmap[17] = 0x00;
player_bitmap[18] = 0x33;
player_bitmap[19] = 0x00;
player_bitmap[20] = 0x75;
player_bitmap[21] = 0x00;
player_bitmap[22] = 0xF5;
player_bitmap[23] = 0x00;
player_bitmap[24] = 0xEB;
player_bitmap[25] = 0x31;
player_bitmap[26] = 0xBF;
player_bitmap[27] = 0xEF;
player_bitmap[28] = 0x3F;
player_bitmap[29] = 0xCF;
player_bitmap[30] = 0xBF;
player_bitmap[31] = 0xEF;
player_bitmap[32] = 0xEB;
player_bitmap[33] = 0x31;
player_bitmap[34] = 0xF5;
player_bitmap[35] = 0x00;
player_bitmap[36] = 0x75;
player_bitmap[37] = 0x00;
player_bitmap[38] = 0x33;
player_bitmap[39] = 0x00;
player_bitmap[40] = 0x2B;
player_bitmap[41] = 0x00;
player_bitmap[42] = 0x76;
player_bitmap[43] = 0x00;
player_bitmap[44] = 0xEC;
player_bitmap[45] = 0x00;
player_bitmap[46] = 0xF4;
player_bitmap[47] = 0x07;
player_bitmap[48] = 0x3E;
player_bitmap[49] = 0x00;
player_bitmap[50] = 0x0F;
player_bitmap[51] = 0x00;
player_bitmap[52] = 0x00;
player_bitmap[53] = 0x00;
player_bitmap[54] = 0x00;
player_bitmap[55] = 0x00;

/* Initialize bomb_bitmap */
bomb_bitmap[0] = 1;
bomb_bitmap[1] = 5;
bomb_bitmap[2] = 0x88;
bomb_bitmap[3] = 0x55;
bomb_bitmap[4] = 0x77;
bomb_bitmap[5] = 0x55;
bomb_bitmap[6] = 0x88;

/* Initialize bullet_bitmap */
bullet_bitmap[0] = 2;
bullet_bitmap[1] = 2;
bullet_bitmap[2] = 0x88;
bullet_bitmap[3] = 0x88;
bullet_bitmap[4] = 0x44;
bullet_bitmap[5] = 0x44;

/* Initialize enemy1_bitmap */
enemy1_bitmap[0] = 2;
enemy1_bitmap[1] = 17;
enemy1_bitmap[2] = 0x00;
enemy1_bitmap[3] = 0x00;
enemy1_bitmap[4] = 0x00;
enemy1_bitmap[5] = 0x0C;
enemy1_bitmap[6] = 0x04;
enemy1_bitmap[7] = 0x1E;
enemy1_bitmap[8] = 0x46;
enemy1_bitmap[9] = 0x3F;
enemy1_bitmap[10] = 0xB8;
enemy1_bitmap[11] = 0x7F;
enemy1_bitmap[12] = 0xB0;
enemy1_bitmap[13] = 0x7F;
enemy1_bitmap[14] = 0xBA;
enemy1_bitmap[15] = 0x7F;
enemy1_bitmap[16] = 0xFD;
enemy1_bitmap[17] = 0x3F;
enemy1_bitmap[18] = 0xFC;
enemy1_bitmap[19] = 0x07;
enemy1_bitmap[20] = 0xFC;
enemy1_bitmap[21] = 0x07;
enemy1_bitmap[22] = 0xFD;
enemy1_bitmap[23] = 0x3F;
enemy1_bitmap[24] = 0xBA;
enemy1_bitmap[25] = 0x7F;
enemy1_bitmap[26] = 0xB0;
enemy1_bitmap[27] = 0x7F;
enemy1_bitmap[28] = 0xB8;
enemy1_bitmap[29] = 0x7F;
enemy1_bitmap[30] = 0x46;
enemy1_bitmap[31] = 0x3F;
enemy1_bitmap[32] = 0x04;
enemy1_bitmap[33] = 0x1E;

/* Initialize enemy2_bitmap */
enemy2_bitmap[0] = 2;
enemy2_bitmap[1] = 16;
enemy2_bitmap[2] = 0x26;
enemy2_bitmap[3] = 0x00;
enemy2_bitmap[4] = 0x59;
enemy2_bitmap[5] = 0x10;
enemy2_bitmap[6] = 0x10;
enemy2_bitmap[7] = 0x30;
enemy2_bitmap[8] = 0x33;
enemy2_bitmap[9] = 0x18;
enemy2_bitmap[10] = 0xE6;
enemy2_bitmap[11] = 0x61;
enemy2_bitmap[12] = 0xC4;
enemy2_bitmap[13] = 0x56;
enemy2_bitmap[14] = 0x03;
enemy2_bitmap[15] = 0x03;
enemy2_bitmap[16] = 0xDC;
enemy2_bitmap[17] = 0x03;
enemy2_bitmap[18] = 0xDC;
enemy2_bitmap[19] = 0x03;
enemy2_bitmap[20] = 0x03;
enemy2_bitmap[21] = 0x03;
enemy2_bitmap[22] = 0xC4;
enemy2_bitmap[23] = 0x56;
enemy2_bitmap[24] = 0xE6;
enemy2_bitmap[25] = 0x61;
enemy2_bitmap[26] = 0x33;
enemy2_bitmap[27] = 0x18;
enemy2_bitmap[28] = 0x10;
enemy2_bitmap[29] = 0x30;
enemy2_bitmap[30] = 0x59;
enemy2_bitmap[31] = 0x10;
enemy2_bitmap[32] = 0x26;
enemy2_bitmap[33] = 0x00;

/* Initialize enemy3_bitmap */
enemy3_bitmap[0] = 2;
enemy3_bitmap[1] = 16;
enemy3_bitmap[2] = 0x80;
enemy3_bitmap[3] = 0x1F;
enemy3_bitmap[4] = 0xC0;
enemy3_bitmap[5] = 0x03;
enemy3_bitmap[6] = 0xF8;
enemy3_bitmap[7] = 0x3F;
enemy3_bitmap[8] = 0x70;
enemy3_bitmap[9] = 0x00;
enemy3_bitmap[10] = 0xF0;
enemy3_bitmap[11] = 0x01;
enemy3_bitmap[12] = 0xFC;
enemy3_bitmap[13] = 0x07;
enemy3_bitmap[14] = 0xE8;
enemy3_bitmap[15] = 0x01;
enemy3_bitmap[16] = 0xF8;
enemy3_bitmap[17] = 0x03;
enemy3_bitmap[18] = 0xF8;
enemy3_bitmap[19] = 0x03;
enemy3_bitmap[20] = 0xE8;
enemy3_bitmap[21] = 0x01;
enemy3_bitmap[22] = 0xF8;
enemy3_bitmap[23] = 0x07;
enemy3_bitmap[24] = 0xF0;
enemy3_bitmap[25] = 0x01;
enemy3_bitmap[26] = 0x70;
enemy3_bitmap[27] = 0x00;
enemy3_bitmap[28] = 0xF8;
enemy3_bitmap[29] = 0x3F;
enemy3_bitmap[30] = 0xC0;
enemy3_bitmap[31] = 0x03;
enemy3_bitmap[32] = 0x80;
enemy3_bitmap[33] = 0x1F;

/* Initialize enemy4_bitmap */
enemy4_bitmap[0] = 2;
enemy4_bitmap[1] = 16;
enemy4_bitmap[2] = 0x06;
enemy4_bitmap[3] = 0x00;
enemy4_bitmap[4] = 0x0C;
enemy4_bitmap[5] = 0x00;
enemy4_bitmap[6] = 0x28;
enemy4_bitmap[7] = 0x00;
enemy4_bitmap[8] = 0x70;
enemy4_bitmap[9] = 0x1F;
enemy4_bitmap[10] = 0x84;
enemy4_bitmap[11] = 0x3F;
enemy4_bitmap[12] = 0xDE;
enemy4_bitmap[13] = 0x37;
enemy4_bitmap[14] = 0xBB;
enemy4_bitmap[15] = 0x3F;
enemy4_bitmap[16] = 0xF0;
enemy4_bitmap[17] = 0x3F;
enemy4_bitmap[18] = 0xF0;
enemy4_bitmap[19] = 0x3F;
enemy4_bitmap[20] = 0xBB;
enemy4_bitmap[21] = 0x3F;
enemy4_bitmap[22] = 0xDE;
enemy4_bitmap[23] = 0x37;
enemy4_bitmap[24] = 0x84;
enemy4_bitmap[25] = 0x3F;
enemy4_bitmap[26] = 0x70;
enemy4_bitmap[27] = 0x1F;
enemy4_bitmap[28] = 0x28;
enemy4_bitmap[29] = 0x00;
enemy4_bitmap[30] = 0x0C;
enemy4_bitmap[31] = 0x00;
enemy4_bitmap[32] = 0x06;
enemy4_bitmap[33] = 0x00;

enemy_bitmaps[0] = enemy1_bitmap;
enemy_bitmaps[1] = enemy2_bitmap;
enemy_bitmaps[2] = enemy3_bitmap;
enemy_bitmaps[3] = enemy4_bitmap;
 
  
font8x8[0 ][0]=0x00;font8x8[0 ][1]=0x00;font8x8[0 ][2]=0x00;font8x8[0 ][3]=0x00;font8x8[0 ][4]=0x00;font8x8[0 ][5]=0x00;font8x8[0 ][6]=0x00;font8x8[0 ][7]=0x00;
font8x8[1 ][0]=0x00;font8x8[1 ][1]=0x00;font8x8[1 ][2]=0x00;font8x8[1 ][3]=0x79;font8x8[1 ][4]=0x79;font8x8[1 ][5]=0x00;font8x8[1 ][6]=0x00;font8x8[1 ][7]=0x00;
font8x8[2 ][0]=0x00;font8x8[2 ][1]=0x70;font8x8[2 ][2]=0x70;font8x8[2 ][3]=0x00;font8x8[2 ][4]=0x00;font8x8[2 ][5]=0x70;font8x8[2 ][6]=0x70;font8x8[2 ][7]=0x00; 
font8x8[3 ][0]=0x14;font8x8[3 ][1]=0x7f;font8x8[3 ][2]=0x7f;font8x8[3 ][3]=0x14;font8x8[3 ][4]=0x14;font8x8[3 ][5]=0x7f;font8x8[3 ][6]=0x7f;font8x8[3 ][7]=0x14; 
font8x8[4 ][0]=0x00;font8x8[4 ][1]=0x12;font8x8[4 ][2]=0x3a;font8x8[4 ][3]=0x6b;font8x8[4 ][4]=0x6b;font8x8[4 ][5]=0x2e;font8x8[4 ][6]=0x24;font8x8[4 ][7]=0x00;
font8x8[5 ][0]=0x00;font8x8[5 ][1]=0x63;font8x8[5 ][2]=0x66;font8x8[5 ][3]=0x0c;font8x8[5 ][4]=0x18;font8x8[5 ][5]=0x33;font8x8[5 ][6]=0x63;font8x8[5 ][7]=0x00; 
font8x8[6 ][0]=0x00;font8x8[6 ][1]=0x26;font8x8[6 ][2]=0x7f;font8x8[6 ][3]=0x59;font8x8[6 ][4]=0x59;font8x8[6 ][5]=0x77;font8x8[6 ][6]=0x27;font8x8[6 ][7]=0x05; 
font8x8[7 ][0]=0x00;font8x8[7 ][1]=0x00;font8x8[7 ][2]=0x00;font8x8[7 ][3]=0x10;font8x8[7 ][4]=0x30;font8x8[7 ][5]=0x60;font8x8[7 ][6]=0x40;font8x8[7 ][7]=0x00; 
font8x8[8 ][0]=0x00;font8x8[8 ][1]=0x00;font8x8[8 ][2]=0x1c;font8x8[8 ][3]=0x3e;font8x8[8 ][4]=0x63;font8x8[8 ][5]=0x41;font8x8[8 ][6]=0x00;font8x8[8 ][7]=0x00; 
font8x8[9 ][0]=0x00;font8x8[9 ][1]=0x00;font8x8[9 ][2]=0x41;font8x8[9 ][3]=0x63;font8x8[9 ][4]=0x3e;font8x8[9 ][5]=0x1c;font8x8[9 ][6]=0x00;font8x8[9 ][7]=0x00; 
font8x8[10][0]=0x08;font8x8[10][1]=0x2a;font8x8[10][2]=0x3e;font8x8[10][3]=0x1c;font8x8[10][4]=0x1c;font8x8[10][5]=0x3e;font8x8[10][6]=0x2a;font8x8[10][7]=0x08; 
font8x8[11][0]=0x00;font8x8[11][1]=0x08;font8x8[11][2]=0x08;font8x8[11][3]=0x3e;font8x8[11][4]=0x3e;font8x8[11][5]=0x08;font8x8[11][6]=0x08;font8x8[11][7]=0x00; 
font8x8[12][0]=0x00;font8x8[12][1]=0x00;font8x8[12][2]=0x00;font8x8[12][3]=0x03;font8x8[12][4]=0x03;font8x8[12][5]=0x00;font8x8[12][6]=0x00;font8x8[12][7]=0x00; 
font8x8[13][0]=0x00;font8x8[13][1]=0x08;font8x8[13][2]=0x08;font8x8[13][3]=0x08;font8x8[13][4]=0x08;font8x8[13][5]=0x08;font8x8[13][6]=0x08;font8x8[13][7]=0x00; 
font8x8[14][0]=0x00;font8x8[14][1]=0x00;font8x8[14][2]=0x00;font8x8[14][3]=0x03;font8x8[14][4]=0x03;font8x8[14][5]=0x00;font8x8[14][6]=0x00;font8x8[14][7]=0x00; 
font8x8[15][0]=0x00;font8x8[15][1]=0x01;font8x8[15][2]=0x03;font8x8[15][3]=0x06;font8x8[15][4]=0x0c;font8x8[15][5]=0x18;font8x8[15][6]=0x30;font8x8[15][7]=0x20; 
font8x8[16][0]=0x00;font8x8[16][1]=0x3e;font8x8[16][2]=0x7f;font8x8[16][3]=0x49;font8x8[16][4]=0x51;font8x8[16][5]=0x7f;font8x8[16][6]=0x3e;font8x8[16][7]=0x00; 
font8x8[17][0]=0x00;font8x8[17][1]=0x01;font8x8[17][2]=0x11;font8x8[17][3]=0x7f;font8x8[17][4]=0x7f;font8x8[17][5]=0x01;font8x8[17][6]=0x01;font8x8[17][7]=0x00; 
font8x8[18][0]=0x00;font8x8[18][1]=0x23;font8x8[18][2]=0x67;font8x8[18][3]=0x45;font8x8[18][4]=0x49;font8x8[18][5]=0x79;font8x8[18][6]=0x31;font8x8[18][7]=0x00; 
font8x8[19][0]=0x00;font8x8[19][1]=0x22;font8x8[19][2]=0x63;font8x8[19][3]=0x49;font8x8[19][4]=0x49;font8x8[19][5]=0x7f;font8x8[19][6]=0x36;font8x8[19][7]=0x00; 
font8x8[20][0]=0x00;font8x8[20][1]=0x0c;font8x8[20][2]=0x0c;font8x8[20][3]=0x14;font8x8[20][4]=0x34;font8x8[20][5]=0x7f;font8x8[20][6]=0x7f;font8x8[20][7]=0x04; 
font8x8[21][0]=0x00;font8x8[21][1]=0x72;font8x8[21][2]=0x73;font8x8[21][3]=0x51;font8x8[21][4]=0x51;font8x8[21][5]=0x5f;font8x8[21][6]=0x4e;font8x8[21][7]=0x00; 
font8x8[22][0]=0x00;font8x8[22][1]=0x3e;font8x8[22][2]=0x7f;font8x8[22][3]=0x49;font8x8[22][4]=0x49;font8x8[22][5]=0x6f;font8x8[22][6]=0x26;font8x8[22][7]=0x00; 
font8x8[23][0]=0x00;font8x8[23][1]=0x60;font8x8[23][2]=0x60;font8x8[23][3]=0x4f;font8x8[23][4]=0x5f;font8x8[23][5]=0x70;font8x8[23][6]=0x60;font8x8[23][7]=0x00; 
font8x8[24][0]=0x00;font8x8[24][1]=0x36;font8x8[24][2]=0x7f;font8x8[24][3]=0x49;font8x8[24][4]=0x49;font8x8[24][5]=0x7f;font8x8[24][6]=0x36;font8x8[24][7]=0x00; 
font8x8[25][0]=0x00;font8x8[25][1]=0x32;font8x8[25][2]=0x7b;font8x8[25][3]=0x49;font8x8[25][4]=0x49;font8x8[25][5]=0x7f;font8x8[25][6]=0x3e;font8x8[25][7]=0x00; 
font8x8[26][0]=0x00;font8x8[26][1]=0x00;font8x8[26][2]=0x00;font8x8[26][3]=0x12;font8x8[26][4]=0x12;font8x8[26][5]=0x00;font8x8[26][6]=0x00;font8x8[26][7]=0x00; 
font8x8[27][0]=0x00;font8x8[27][1]=0x00;font8x8[27][2]=0x00;font8x8[27][3]=0x13;font8x8[27][4]=0x13;font8x8[27][5]=0x00;font8x8[27][6]=0x00;font8x8[27][7]=0x00; 
font8x8[28][0]=0x00;font8x8[28][1]=0x08;font8x8[28][2]=0x1c;font8x8[28][3]=0x36;font8x8[28][4]=0x63;font8x8[28][5]=0x41;font8x8[28][6]=0x41;font8x8[28][7]=0x00; 
font8x8[29][0]=0x00;font8x8[29][1]=0x14;font8x8[29][2]=0x14;font8x8[29][3]=0x14;font8x8[29][4]=0x14;font8x8[29][5]=0x14;font8x8[29][6]=0x14;font8x8[29][7]=0x00; 
font8x8[30][0]=0x00;font8x8[30][1]=0x41;font8x8[30][2]=0x41;font8x8[30][3]=0x63;font8x8[30][4]=0x36;font8x8[30][5]=0x1c;font8x8[30][6]=0x08;font8x8[30][7]=0x00; 
font8x8[31][0]=0x00;font8x8[31][1]=0x20;font8x8[31][2]=0x60;font8x8[31][3]=0x45;font8x8[31][4]=0x4d;font8x8[31][5]=0x78;font8x8[31][6]=0x30;font8x8[31][7]=0x00; 
font8x8[32][0]=0x00;font8x8[32][1]=0x3e;font8x8[32][2]=0x7f;font8x8[32][3]=0x41;font8x8[32][4]=0x59;font8x8[32][5]=0x79;font8x8[32][6]=0x3a;font8x8[32][7]=0x00; 
font8x8[33][0]=0x00;font8x8[33][1]=0x1f;font8x8[33][2]=0x3f;font8x8[33][3]=0x68;font8x8[33][4]=0x68;font8x8[33][5]=0x3f;font8x8[33][6]=0x1f;font8x8[33][7]=0x00; 
font8x8[34][0]=0x00;font8x8[34][1]=0x7f;font8x8[34][2]=0x7f;font8x8[34][3]=0x49;font8x8[34][4]=0x49;font8x8[34][5]=0x7f;font8x8[34][6]=0x36;font8x8[34][7]=0x00; 
font8x8[35][0]=0x00;font8x8[35][1]=0x3e;font8x8[35][2]=0x7f;font8x8[35][3]=0x41;font8x8[35][4]=0x41;font8x8[35][5]=0x63;font8x8[35][6]=0x22;font8x8[35][7]=0x00; 
font8x8[36][0]=0x00;font8x8[36][1]=0x7f;font8x8[36][2]=0x7f;font8x8[36][3]=0x41;font8x8[36][4]=0x63;font8x8[36][5]=0x3e;font8x8[36][6]=0x1c;font8x8[36][7]=0x00; 
font8x8[37][0]=0x00;font8x8[37][1]=0x7f;font8x8[37][2]=0x7f;font8x8[37][3]=0x49;font8x8[37][4]=0x49;font8x8[37][5]=0x41;font8x8[37][6]=0x41;font8x8[37][7]=0x00; 
font8x8[38][0]=0x00;font8x8[38][1]=0x7f;font8x8[38][2]=0x7f;font8x8[38][3]=0x48;font8x8[38][4]=0x48;font8x8[38][5]=0x40;font8x8[38][6]=0x40;font8x8[38][7]=0x00; 
font8x8[39][0]=0x00;font8x8[39][1]=0x3e;font8x8[39][2]=0x7f;font8x8[39][3]=0x41;font8x8[39][4]=0x49;font8x8[39][5]=0x6f;font8x8[39][6]=0x2e;font8x8[39][7]=0x00; 
font8x8[40][0]=0x00;font8x8[40][1]=0x7f;font8x8[40][2]=0x7f;font8x8[40][3]=0x08;font8x8[40][4]=0x08;font8x8[40][5]=0x7f;font8x8[40][6]=0x7f;font8x8[40][7]=0x00; 
font8x8[41][0]=0x00;font8x8[41][1]=0x00;font8x8[41][2]=0x41;font8x8[41][3]=0x7f;font8x8[41][4]=0x7f;font8x8[41][5]=0x41;font8x8[41][6]=0x00;font8x8[41][7]=0x00; 
font8x8[42][0]=0x00;font8x8[42][1]=0x02;font8x8[42][2]=0x03;font8x8[42][3]=0x41;font8x8[42][4]=0x7f;font8x8[42][5]=0x7e;font8x8[42][6]=0x40;font8x8[42][7]=0x00; 
font8x8[43][0]=0x00;font8x8[43][1]=0x7f;font8x8[43][2]=0x7f;font8x8[43][3]=0x1c;font8x8[43][4]=0x36;font8x8[43][5]=0x63;font8x8[43][6]=0x41;font8x8[43][7]=0x00; 
font8x8[44][0]=0x00;font8x8[44][1]=0x7f;font8x8[44][2]=0x7f;font8x8[44][3]=0x01;font8x8[44][4]=0x01;font8x8[44][5]=0x01;font8x8[44][6]=0x01;font8x8[44][7]=0x00; 
font8x8[45][0]=0x00;font8x8[45][1]=0x7f;font8x8[45][2]=0x7f;font8x8[45][3]=0x30;font8x8[45][4]=0x18;font8x8[45][5]=0x30;font8x8[45][6]=0x7f;font8x8[45][7]=0x7f; 
font8x8[46][0]=0x00;font8x8[46][1]=0x7f;font8x8[46][2]=0x7f;font8x8[46][3]=0x38;font8x8[46][4]=0x1c;font8x8[46][5]=0x7f;font8x8[46][6]=0x7f;font8x8[46][7]=0x00; 
font8x8[47][0]=0x00;font8x8[47][1]=0x3e;font8x8[47][2]=0x7f;font8x8[47][3]=0x41;font8x8[47][4]=0x41;font8x8[47][5]=0x7f;font8x8[47][6]=0x3e;font8x8[47][7]=0x00; 
font8x8[48][0]=0x00;font8x8[48][1]=0x7f;font8x8[48][2]=0x7f;font8x8[48][3]=0x48;font8x8[48][4]=0x48;font8x8[48][5]=0x78;font8x8[48][6]=0x30;font8x8[48][7]=0x00; 
font8x8[49][0]=0x00;font8x8[49][1]=0x3c;font8x8[49][2]=0x7e;font8x8[49][3]=0x42;font8x8[49][4]=0x43;font8x8[49][5]=0x7f;font8x8[49][6]=0x3d;font8x8[49][7]=0x00; 
font8x8[50][0]=0x00;font8x8[50][1]=0x7f;font8x8[50][2]=0x7f;font8x8[50][3]=0x4c;font8x8[50][4]=0x4e;font8x8[50][5]=0x7b;font8x8[50][6]=0x31;font8x8[50][7]=0x00; 
font8x8[51][0]=0x00;font8x8[51][1]=0x32;font8x8[51][2]=0x7b;font8x8[51][3]=0x49;font8x8[51][4]=0x49;font8x8[51][5]=0x6f;font8x8[51][6]=0x26;font8x8[51][7]=0x00; 
font8x8[52][0]=0x00;font8x8[52][1]=0x40;font8x8[52][2]=0x40;font8x8[52][3]=0x7f;font8x8[52][4]=0x7f;font8x8[52][5]=0x40;font8x8[52][6]=0x40;font8x8[52][7]=0x00; 
font8x8[53][0]=0x00;font8x8[53][1]=0x7e;font8x8[53][2]=0x7f;font8x8[53][3]=0x01;font8x8[53][4]=0x01;font8x8[53][5]=0x7f;font8x8[53][6]=0x7e;font8x8[53][7]=0x00; 
font8x8[54][0]=0x00;font8x8[54][1]=0x7c;font8x8[54][2]=0x7e;font8x8[54][3]=0x03;font8x8[54][4]=0x03;font8x8[54][5]=0x7e;font8x8[54][6]=0x7c;font8x8[54][7]=0x00; 
font8x8[55][0]=0x00;font8x8[55][1]=0x7f;font8x8[55][2]=0x7f;font8x8[55][3]=0x06;font8x8[55][4]=0x0c;font8x8[55][5]=0x06;font8x8[55][6]=0x7f;font8x8[55][7]=0x7f; 
font8x8[56][0]=0x00;font8x8[56][1]=0x63;font8x8[56][2]=0x77;font8x8[56][3]=0x1c;font8x8[56][4]=0x1c;font8x8[56][5]=0x77;font8x8[56][6]=0x63;font8x8[56][7]=0x00; 
font8x8[57][0]=0x00;font8x8[57][1]=0x70;font8x8[57][2]=0x78;font8x8[57][3]=0x0f;font8x8[57][4]=0x0f;font8x8[57][5]=0x78;font8x8[57][6]=0x70;font8x8[57][7]=0x00; 
font8x8[58][0]=0x00;font8x8[58][1]=0x43;font8x8[58][2]=0x47;font8x8[58][3]=0x4d;font8x8[58][4]=0x59;font8x8[58][5]=0x71;font8x8[58][6]=0x61;font8x8[58][7]=0x00; 
font8x8[59][0]=0x00;font8x8[59][1]=0x00;font8x8[59][2]=0x7f;font8x8[59][3]=0x7f;font8x8[59][4]=0x41;font8x8[59][5]=0x41;font8x8[59][6]=0x00;font8x8[59][7]=0x00; 
font8x8[60][0]=0x00;font8x8[60][1]=0x20;font8x8[60][2]=0x30;font8x8[60][3]=0x18;font8x8[60][4]=0x0c;font8x8[60][5]=0x06;font8x8[60][6]=0x03;font8x8[60][7]=0x01; 
font8x8[61][0]=0x00;font8x8[61][1]=0x00;font8x8[61][2]=0x41;font8x8[61][3]=0x41;font8x8[61][4]=0x7f;font8x8[61][5]=0x7f;font8x8[61][6]=0x00;font8x8[61][7]=0x00; 
font8x8[62][0]=0x00;font8x8[62][1]=0x08;font8x8[62][2]=0x18;font8x8[62][3]=0x3f;font8x8[62][4]=0x3f;font8x8[62][5]=0x18;font8x8[62][6]=0x08;font8x8[62][7]=0x00;
  
}

void cosmic_impalas_main() {
  // NOTE: initializers don't get run, so we init here
  FIRE1  = 0;
  LEFT1  = 0;
  RIGHT1 = 0;
  srand(1234);
  initialize_all_bitmaps();
  credits = 0;
  play_game(); 
}
