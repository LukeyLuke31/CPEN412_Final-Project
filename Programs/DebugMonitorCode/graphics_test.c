
#include <string.h>
#define PIXEL_BUFFER_VGA_BASEADDRESS   (0x01000000)
#define VIDMEM_DIM1_W_BLANK_EDGES (256)
#define VIDMEM_DIM1 (224)
#define VIDMEM_DIM2 (32)

#define VIDEOMEM_ADDR(x,y) ((volatile unsigned char *)(PIXEL_BUFFER_VGA_BASEADDRESS + ((y)*VIDMEM_DIM1_W_BLANK_EDGES)+(x)))
#define VIDMEM(x,y) (*VIDEOMEM_ADDR(x,y))

#define WRITE_VIDMEM(x,y,CH) ((*(volatile unsigned char *)(PIXEL_BUFFER_VGA_BASEADDRESS + ((y)*VIDMEM_DIM1_W_BLANK_EDGES)+(x))) = CH)

#define LOCHAR 0x20
#define HICHAR 0x5e
char graphics_font8x8[HICHAR-LOCHAR+1][8];

void graphics_test_main();

/// GRAPHICS FUNCTIONS

void graphics_test_clrscr() {
  int a;
  int b;
  for (a = 0; a < VIDMEM_DIM1; a++) {
    for (b = 0; b < VIDMEM_DIM2; b++) {
       WRITE_VIDMEM(a,b,0 /*0xff*/);	   
    }
  }
}

void graphics_xor_pixel(unsigned char x, unsigned char y) {
  unsigned char* dest;
  dest =  VIDEOMEM_ADDR(x,y>>3);
  *dest = (*dest) ^ (0x1 << (y&7));
}

void graphics_draw_vline(unsigned char x, unsigned char y1, unsigned char y2) {
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





void graphics_draw_char(unsigned char ch, unsigned char x, unsigned char y) {
  unsigned char i;
  unsigned char* src;
  unsigned char* dest;
  src  = &graphics_font8x8[(ch-LOCHAR)][0];
  dest = VIDEOMEM_ADDR(x*8,y);
  
  for (i=0; i<8; i++) {
    *dest = *src;
    dest += 1;//32;
    src += 1;
  }
}

void graphics_draw_string(const char* str, unsigned char x, unsigned char y) {
  do {
    unsigned char ch;
	ch = *str++;
    if (!ch) break;
    graphics_draw_char(ch, x, y);
    x++;
  } while (1);
}

void draw_font() {
  unsigned char i;
  i=LOCHAR;
  do {
    graphics_draw_char(i, i&15, 31-(i>>4));
    graphics_draw_vline(i, i, i*2);
    graphics_xor_pixel(i*15, i);
  } while (++i != HICHAR);
}

void initialize_font() {
	graphics_font8x8[0 ][0]=0x00;graphics_font8x8[0 ][1]=0x00;graphics_font8x8[0 ][2]=0x00;graphics_font8x8[0 ][3]=0x00;graphics_font8x8[0 ][4]=0x00;graphics_font8x8[0 ][5]=0x00;graphics_font8x8[0 ][6]=0x00;graphics_font8x8[0 ][7]=0x00;
	graphics_font8x8[1 ][0]=0x00;graphics_font8x8[1 ][1]=0x00;graphics_font8x8[1 ][2]=0x00;graphics_font8x8[1 ][3]=0x79;graphics_font8x8[1 ][4]=0x79;graphics_font8x8[1 ][5]=0x00;graphics_font8x8[1 ][6]=0x00;graphics_font8x8[1 ][7]=0x00;
	graphics_font8x8[2 ][0]=0x00;graphics_font8x8[2 ][1]=0x70;graphics_font8x8[2 ][2]=0x70;graphics_font8x8[2 ][3]=0x00;graphics_font8x8[2 ][4]=0x00;graphics_font8x8[2 ][5]=0x70;graphics_font8x8[2 ][6]=0x70;graphics_font8x8[2 ][7]=0x00; 
	graphics_font8x8[3 ][0]=0x14;graphics_font8x8[3 ][1]=0x7f;graphics_font8x8[3 ][2]=0x7f;graphics_font8x8[3 ][3]=0x14;graphics_font8x8[3 ][4]=0x14;graphics_font8x8[3 ][5]=0x7f;graphics_font8x8[3 ][6]=0x7f;graphics_font8x8[3 ][7]=0x14; 
	graphics_font8x8[4 ][0]=0x00;graphics_font8x8[4 ][1]=0x12;graphics_font8x8[4 ][2]=0x3a;graphics_font8x8[4 ][3]=0x6b;graphics_font8x8[4 ][4]=0x6b;graphics_font8x8[4 ][5]=0x2e;graphics_font8x8[4 ][6]=0x24;graphics_font8x8[4 ][7]=0x00;
	graphics_font8x8[5 ][0]=0x00;graphics_font8x8[5 ][1]=0x63;graphics_font8x8[5 ][2]=0x66;graphics_font8x8[5 ][3]=0x0c;graphics_font8x8[5 ][4]=0x18;graphics_font8x8[5 ][5]=0x33;graphics_font8x8[5 ][6]=0x63;graphics_font8x8[5 ][7]=0x00; 
	graphics_font8x8[6 ][0]=0x00;graphics_font8x8[6 ][1]=0x26;graphics_font8x8[6 ][2]=0x7f;graphics_font8x8[6 ][3]=0x59;graphics_font8x8[6 ][4]=0x59;graphics_font8x8[6 ][5]=0x77;graphics_font8x8[6 ][6]=0x27;graphics_font8x8[6 ][7]=0x05; 
	graphics_font8x8[7 ][0]=0x00;graphics_font8x8[7 ][1]=0x00;graphics_font8x8[7 ][2]=0x00;graphics_font8x8[7 ][3]=0x10;graphics_font8x8[7 ][4]=0x30;graphics_font8x8[7 ][5]=0x60;graphics_font8x8[7 ][6]=0x40;graphics_font8x8[7 ][7]=0x00; 
	graphics_font8x8[8 ][0]=0x00;graphics_font8x8[8 ][1]=0x00;graphics_font8x8[8 ][2]=0x1c;graphics_font8x8[8 ][3]=0x3e;graphics_font8x8[8 ][4]=0x63;graphics_font8x8[8 ][5]=0x41;graphics_font8x8[8 ][6]=0x00;graphics_font8x8[8 ][7]=0x00; 
	graphics_font8x8[9 ][0]=0x00;graphics_font8x8[9 ][1]=0x00;graphics_font8x8[9 ][2]=0x41;graphics_font8x8[9 ][3]=0x63;graphics_font8x8[9 ][4]=0x3e;graphics_font8x8[9 ][5]=0x1c;graphics_font8x8[9 ][6]=0x00;graphics_font8x8[9 ][7]=0x00; 
	graphics_font8x8[10][0]=0x08;graphics_font8x8[10][1]=0x2a;graphics_font8x8[10][2]=0x3e;graphics_font8x8[10][3]=0x1c;graphics_font8x8[10][4]=0x1c;graphics_font8x8[10][5]=0x3e;graphics_font8x8[10][6]=0x2a;graphics_font8x8[10][7]=0x08; 
	graphics_font8x8[11][0]=0x00;graphics_font8x8[11][1]=0x08;graphics_font8x8[11][2]=0x08;graphics_font8x8[11][3]=0x3e;graphics_font8x8[11][4]=0x3e;graphics_font8x8[11][5]=0x08;graphics_font8x8[11][6]=0x08;graphics_font8x8[11][7]=0x00; 
	graphics_font8x8[12][0]=0x00;graphics_font8x8[12][1]=0x00;graphics_font8x8[12][2]=0x00;graphics_font8x8[12][3]=0x03;graphics_font8x8[12][4]=0x03;graphics_font8x8[12][5]=0x00;graphics_font8x8[12][6]=0x00;graphics_font8x8[12][7]=0x00; 
	graphics_font8x8[13][0]=0x00;graphics_font8x8[13][1]=0x08;graphics_font8x8[13][2]=0x08;graphics_font8x8[13][3]=0x08;graphics_font8x8[13][4]=0x08;graphics_font8x8[13][5]=0x08;graphics_font8x8[13][6]=0x08;graphics_font8x8[13][7]=0x00; 
	graphics_font8x8[14][0]=0x00;graphics_font8x8[14][1]=0x00;graphics_font8x8[14][2]=0x00;graphics_font8x8[14][3]=0x03;graphics_font8x8[14][4]=0x03;graphics_font8x8[14][5]=0x00;graphics_font8x8[14][6]=0x00;graphics_font8x8[14][7]=0x00; 
	graphics_font8x8[15][0]=0x00;graphics_font8x8[15][1]=0x01;graphics_font8x8[15][2]=0x03;graphics_font8x8[15][3]=0x06;graphics_font8x8[15][4]=0x0c;graphics_font8x8[15][5]=0x18;graphics_font8x8[15][6]=0x30;graphics_font8x8[15][7]=0x20; 
	graphics_font8x8[16][0]=0x00;graphics_font8x8[16][1]=0x3e;graphics_font8x8[16][2]=0x7f;graphics_font8x8[16][3]=0x49;graphics_font8x8[16][4]=0x51;graphics_font8x8[16][5]=0x7f;graphics_font8x8[16][6]=0x3e;graphics_font8x8[16][7]=0x00; 
	graphics_font8x8[17][0]=0x00;graphics_font8x8[17][1]=0x01;graphics_font8x8[17][2]=0x11;graphics_font8x8[17][3]=0x7f;graphics_font8x8[17][4]=0x7f;graphics_font8x8[17][5]=0x01;graphics_font8x8[17][6]=0x01;graphics_font8x8[17][7]=0x00; 
	graphics_font8x8[18][0]=0x00;graphics_font8x8[18][1]=0x23;graphics_font8x8[18][2]=0x67;graphics_font8x8[18][3]=0x45;graphics_font8x8[18][4]=0x49;graphics_font8x8[18][5]=0x79;graphics_font8x8[18][6]=0x31;graphics_font8x8[18][7]=0x00; 
	graphics_font8x8[19][0]=0x00;graphics_font8x8[19][1]=0x22;graphics_font8x8[19][2]=0x63;graphics_font8x8[19][3]=0x49;graphics_font8x8[19][4]=0x49;graphics_font8x8[19][5]=0x7f;graphics_font8x8[19][6]=0x36;graphics_font8x8[19][7]=0x00; 
	graphics_font8x8[20][0]=0x00;graphics_font8x8[20][1]=0x0c;graphics_font8x8[20][2]=0x0c;graphics_font8x8[20][3]=0x14;graphics_font8x8[20][4]=0x34;graphics_font8x8[20][5]=0x7f;graphics_font8x8[20][6]=0x7f;graphics_font8x8[20][7]=0x04; 
	graphics_font8x8[21][0]=0x00;graphics_font8x8[21][1]=0x72;graphics_font8x8[21][2]=0x73;graphics_font8x8[21][3]=0x51;graphics_font8x8[21][4]=0x51;graphics_font8x8[21][5]=0x5f;graphics_font8x8[21][6]=0x4e;graphics_font8x8[21][7]=0x00; 
	graphics_font8x8[22][0]=0x00;graphics_font8x8[22][1]=0x3e;graphics_font8x8[22][2]=0x7f;graphics_font8x8[22][3]=0x49;graphics_font8x8[22][4]=0x49;graphics_font8x8[22][5]=0x6f;graphics_font8x8[22][6]=0x26;graphics_font8x8[22][7]=0x00; 
	graphics_font8x8[23][0]=0x00;graphics_font8x8[23][1]=0x60;graphics_font8x8[23][2]=0x60;graphics_font8x8[23][3]=0x4f;graphics_font8x8[23][4]=0x5f;graphics_font8x8[23][5]=0x70;graphics_font8x8[23][6]=0x60;graphics_font8x8[23][7]=0x00; 
	graphics_font8x8[24][0]=0x00;graphics_font8x8[24][1]=0x36;graphics_font8x8[24][2]=0x7f;graphics_font8x8[24][3]=0x49;graphics_font8x8[24][4]=0x49;graphics_font8x8[24][5]=0x7f;graphics_font8x8[24][6]=0x36;graphics_font8x8[24][7]=0x00; 
	graphics_font8x8[25][0]=0x00;graphics_font8x8[25][1]=0x32;graphics_font8x8[25][2]=0x7b;graphics_font8x8[25][3]=0x49;graphics_font8x8[25][4]=0x49;graphics_font8x8[25][5]=0x7f;graphics_font8x8[25][6]=0x3e;graphics_font8x8[25][7]=0x00; 
	graphics_font8x8[26][0]=0x00;graphics_font8x8[26][1]=0x00;graphics_font8x8[26][2]=0x00;graphics_font8x8[26][3]=0x12;graphics_font8x8[26][4]=0x12;graphics_font8x8[26][5]=0x00;graphics_font8x8[26][6]=0x00;graphics_font8x8[26][7]=0x00; 
	graphics_font8x8[27][0]=0x00;graphics_font8x8[27][1]=0x00;graphics_font8x8[27][2]=0x00;graphics_font8x8[27][3]=0x13;graphics_font8x8[27][4]=0x13;graphics_font8x8[27][5]=0x00;graphics_font8x8[27][6]=0x00;graphics_font8x8[27][7]=0x00; 
	graphics_font8x8[28][0]=0x00;graphics_font8x8[28][1]=0x08;graphics_font8x8[28][2]=0x1c;graphics_font8x8[28][3]=0x36;graphics_font8x8[28][4]=0x63;graphics_font8x8[28][5]=0x41;graphics_font8x8[28][6]=0x41;graphics_font8x8[28][7]=0x00; 
	graphics_font8x8[29][0]=0x00;graphics_font8x8[29][1]=0x14;graphics_font8x8[29][2]=0x14;graphics_font8x8[29][3]=0x14;graphics_font8x8[29][4]=0x14;graphics_font8x8[29][5]=0x14;graphics_font8x8[29][6]=0x14;graphics_font8x8[29][7]=0x00; 
	graphics_font8x8[30][0]=0x00;graphics_font8x8[30][1]=0x41;graphics_font8x8[30][2]=0x41;graphics_font8x8[30][3]=0x63;graphics_font8x8[30][4]=0x36;graphics_font8x8[30][5]=0x1c;graphics_font8x8[30][6]=0x08;graphics_font8x8[30][7]=0x00; 
	graphics_font8x8[31][0]=0x00;graphics_font8x8[31][1]=0x20;graphics_font8x8[31][2]=0x60;graphics_font8x8[31][3]=0x45;graphics_font8x8[31][4]=0x4d;graphics_font8x8[31][5]=0x78;graphics_font8x8[31][6]=0x30;graphics_font8x8[31][7]=0x00; 
	graphics_font8x8[32][0]=0x00;graphics_font8x8[32][1]=0x3e;graphics_font8x8[32][2]=0x7f;graphics_font8x8[32][3]=0x41;graphics_font8x8[32][4]=0x59;graphics_font8x8[32][5]=0x79;graphics_font8x8[32][6]=0x3a;graphics_font8x8[32][7]=0x00; 
	graphics_font8x8[33][0]=0x00;graphics_font8x8[33][1]=0x1f;graphics_font8x8[33][2]=0x3f;graphics_font8x8[33][3]=0x68;graphics_font8x8[33][4]=0x68;graphics_font8x8[33][5]=0x3f;graphics_font8x8[33][6]=0x1f;graphics_font8x8[33][7]=0x00; 
	graphics_font8x8[34][0]=0x00;graphics_font8x8[34][1]=0x7f;graphics_font8x8[34][2]=0x7f;graphics_font8x8[34][3]=0x49;graphics_font8x8[34][4]=0x49;graphics_font8x8[34][5]=0x7f;graphics_font8x8[34][6]=0x36;graphics_font8x8[34][7]=0x00; 
	graphics_font8x8[35][0]=0x00;graphics_font8x8[35][1]=0x3e;graphics_font8x8[35][2]=0x7f;graphics_font8x8[35][3]=0x41;graphics_font8x8[35][4]=0x41;graphics_font8x8[35][5]=0x63;graphics_font8x8[35][6]=0x22;graphics_font8x8[35][7]=0x00; 
	graphics_font8x8[36][0]=0x00;graphics_font8x8[36][1]=0x7f;graphics_font8x8[36][2]=0x7f;graphics_font8x8[36][3]=0x41;graphics_font8x8[36][4]=0x63;graphics_font8x8[36][5]=0x3e;graphics_font8x8[36][6]=0x1c;graphics_font8x8[36][7]=0x00; 
	graphics_font8x8[37][0]=0x00;graphics_font8x8[37][1]=0x7f;graphics_font8x8[37][2]=0x7f;graphics_font8x8[37][3]=0x49;graphics_font8x8[37][4]=0x49;graphics_font8x8[37][5]=0x41;graphics_font8x8[37][6]=0x41;graphics_font8x8[37][7]=0x00; 
	graphics_font8x8[38][0]=0x00;graphics_font8x8[38][1]=0x7f;graphics_font8x8[38][2]=0x7f;graphics_font8x8[38][3]=0x48;graphics_font8x8[38][4]=0x48;graphics_font8x8[38][5]=0x40;graphics_font8x8[38][6]=0x40;graphics_font8x8[38][7]=0x00; 
	graphics_font8x8[39][0]=0x00;graphics_font8x8[39][1]=0x3e;graphics_font8x8[39][2]=0x7f;graphics_font8x8[39][3]=0x41;graphics_font8x8[39][4]=0x49;graphics_font8x8[39][5]=0x6f;graphics_font8x8[39][6]=0x2e;graphics_font8x8[39][7]=0x00; 
	graphics_font8x8[40][0]=0x00;graphics_font8x8[40][1]=0x7f;graphics_font8x8[40][2]=0x7f;graphics_font8x8[40][3]=0x08;graphics_font8x8[40][4]=0x08;graphics_font8x8[40][5]=0x7f;graphics_font8x8[40][6]=0x7f;graphics_font8x8[40][7]=0x00; 
	graphics_font8x8[41][0]=0x00;graphics_font8x8[41][1]=0x00;graphics_font8x8[41][2]=0x41;graphics_font8x8[41][3]=0x7f;graphics_font8x8[41][4]=0x7f;graphics_font8x8[41][5]=0x41;graphics_font8x8[41][6]=0x00;graphics_font8x8[41][7]=0x00; 
	graphics_font8x8[42][0]=0x00;graphics_font8x8[42][1]=0x02;graphics_font8x8[42][2]=0x03;graphics_font8x8[42][3]=0x41;graphics_font8x8[42][4]=0x7f;graphics_font8x8[42][5]=0x7e;graphics_font8x8[42][6]=0x40;graphics_font8x8[42][7]=0x00; 
	graphics_font8x8[43][0]=0x00;graphics_font8x8[43][1]=0x7f;graphics_font8x8[43][2]=0x7f;graphics_font8x8[43][3]=0x1c;graphics_font8x8[43][4]=0x36;graphics_font8x8[43][5]=0x63;graphics_font8x8[43][6]=0x41;graphics_font8x8[43][7]=0x00; 
	graphics_font8x8[44][0]=0x00;graphics_font8x8[44][1]=0x7f;graphics_font8x8[44][2]=0x7f;graphics_font8x8[44][3]=0x01;graphics_font8x8[44][4]=0x01;graphics_font8x8[44][5]=0x01;graphics_font8x8[44][6]=0x01;graphics_font8x8[44][7]=0x00; 
	graphics_font8x8[45][0]=0x00;graphics_font8x8[45][1]=0x7f;graphics_font8x8[45][2]=0x7f;graphics_font8x8[45][3]=0x30;graphics_font8x8[45][4]=0x18;graphics_font8x8[45][5]=0x30;graphics_font8x8[45][6]=0x7f;graphics_font8x8[45][7]=0x7f; 
	graphics_font8x8[46][0]=0x00;graphics_font8x8[46][1]=0x7f;graphics_font8x8[46][2]=0x7f;graphics_font8x8[46][3]=0x38;graphics_font8x8[46][4]=0x1c;graphics_font8x8[46][5]=0x7f;graphics_font8x8[46][6]=0x7f;graphics_font8x8[46][7]=0x00; 
	graphics_font8x8[47][0]=0x00;graphics_font8x8[47][1]=0x3e;graphics_font8x8[47][2]=0x7f;graphics_font8x8[47][3]=0x41;graphics_font8x8[47][4]=0x41;graphics_font8x8[47][5]=0x7f;graphics_font8x8[47][6]=0x3e;graphics_font8x8[47][7]=0x00; 
	graphics_font8x8[48][0]=0x00;graphics_font8x8[48][1]=0x7f;graphics_font8x8[48][2]=0x7f;graphics_font8x8[48][3]=0x48;graphics_font8x8[48][4]=0x48;graphics_font8x8[48][5]=0x78;graphics_font8x8[48][6]=0x30;graphics_font8x8[48][7]=0x00; 
	graphics_font8x8[49][0]=0x00;graphics_font8x8[49][1]=0x3c;graphics_font8x8[49][2]=0x7e;graphics_font8x8[49][3]=0x42;graphics_font8x8[49][4]=0x43;graphics_font8x8[49][5]=0x7f;graphics_font8x8[49][6]=0x3d;graphics_font8x8[49][7]=0x00; 
	graphics_font8x8[50][0]=0x00;graphics_font8x8[50][1]=0x7f;graphics_font8x8[50][2]=0x7f;graphics_font8x8[50][3]=0x4c;graphics_font8x8[50][4]=0x4e;graphics_font8x8[50][5]=0x7b;graphics_font8x8[50][6]=0x31;graphics_font8x8[50][7]=0x00; 
	graphics_font8x8[51][0]=0x00;graphics_font8x8[51][1]=0x32;graphics_font8x8[51][2]=0x7b;graphics_font8x8[51][3]=0x49;graphics_font8x8[51][4]=0x49;graphics_font8x8[51][5]=0x6f;graphics_font8x8[51][6]=0x26;graphics_font8x8[51][7]=0x00; 
	graphics_font8x8[52][0]=0x00;graphics_font8x8[52][1]=0x40;graphics_font8x8[52][2]=0x40;graphics_font8x8[52][3]=0x7f;graphics_font8x8[52][4]=0x7f;graphics_font8x8[52][5]=0x40;graphics_font8x8[52][6]=0x40;graphics_font8x8[52][7]=0x00; 
	graphics_font8x8[53][0]=0x00;graphics_font8x8[53][1]=0x7e;graphics_font8x8[53][2]=0x7f;graphics_font8x8[53][3]=0x01;graphics_font8x8[53][4]=0x01;graphics_font8x8[53][5]=0x7f;graphics_font8x8[53][6]=0x7e;graphics_font8x8[53][7]=0x00; 
	graphics_font8x8[54][0]=0x00;graphics_font8x8[54][1]=0x7c;graphics_font8x8[54][2]=0x7e;graphics_font8x8[54][3]=0x03;graphics_font8x8[54][4]=0x03;graphics_font8x8[54][5]=0x7e;graphics_font8x8[54][6]=0x7c;graphics_font8x8[54][7]=0x00; 
	graphics_font8x8[55][0]=0x00;graphics_font8x8[55][1]=0x7f;graphics_font8x8[55][2]=0x7f;graphics_font8x8[55][3]=0x06;graphics_font8x8[55][4]=0x0c;graphics_font8x8[55][5]=0x06;graphics_font8x8[55][6]=0x7f;graphics_font8x8[55][7]=0x7f; 
	graphics_font8x8[56][0]=0x00;graphics_font8x8[56][1]=0x63;graphics_font8x8[56][2]=0x77;graphics_font8x8[56][3]=0x1c;graphics_font8x8[56][4]=0x1c;graphics_font8x8[56][5]=0x77;graphics_font8x8[56][6]=0x63;graphics_font8x8[56][7]=0x00; 
	graphics_font8x8[57][0]=0x00;graphics_font8x8[57][1]=0x70;graphics_font8x8[57][2]=0x78;graphics_font8x8[57][3]=0x0f;graphics_font8x8[57][4]=0x0f;graphics_font8x8[57][5]=0x78;graphics_font8x8[57][6]=0x70;graphics_font8x8[57][7]=0x00; 
	graphics_font8x8[58][0]=0x00;graphics_font8x8[58][1]=0x43;graphics_font8x8[58][2]=0x47;graphics_font8x8[58][3]=0x4d;graphics_font8x8[58][4]=0x59;graphics_font8x8[58][5]=0x71;graphics_font8x8[58][6]=0x61;graphics_font8x8[58][7]=0x00; 
	graphics_font8x8[59][0]=0x00;graphics_font8x8[59][1]=0x00;graphics_font8x8[59][2]=0x7f;graphics_font8x8[59][3]=0x7f;graphics_font8x8[59][4]=0x41;graphics_font8x8[59][5]=0x41;graphics_font8x8[59][6]=0x00;graphics_font8x8[59][7]=0x00; 
	graphics_font8x8[60][0]=0x00;graphics_font8x8[60][1]=0x20;graphics_font8x8[60][2]=0x30;graphics_font8x8[60][3]=0x18;graphics_font8x8[60][4]=0x0c;graphics_font8x8[60][5]=0x06;graphics_font8x8[60][6]=0x03;graphics_font8x8[60][7]=0x01; 
	graphics_font8x8[61][0]=0x00;graphics_font8x8[61][1]=0x00;graphics_font8x8[61][2]=0x41;graphics_font8x8[61][3]=0x41;graphics_font8x8[61][4]=0x7f;graphics_font8x8[61][5]=0x7f;graphics_font8x8[61][6]=0x00;graphics_font8x8[61][7]=0x00; 
	graphics_font8x8[62][0]=0x00;graphics_font8x8[62][1]=0x08;graphics_font8x8[62][2]=0x18;graphics_font8x8[62][3]=0x3f;graphics_font8x8[62][4]=0x3f;graphics_font8x8[62][5]=0x18;graphics_font8x8[62][6]=0x08;graphics_font8x8[62][7]=0x00;	
}
void graphics_test_main() {
  initialize_font();
  graphics_test_clrscr();
  draw_font();
  graphics_draw_string("HELLO WORLD", 0, 0);
}