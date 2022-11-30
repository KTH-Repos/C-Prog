/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from mipslabfunc.c */
void display_init(void);
void display_string(int line, char *s);
void display_font_update(void);
void display_font_clear(void);
uint8_t spi_send_recv(uint8_t data);

/* Declare lab-related functions from mipslabfunc.c */
char * itoaconv( int num );
void quicksleep(int cyc);
void tick( unsigned int * timep );

/* Declare game-related functions */
extern int timePress;
void runGame(void);
void playInit(void);
void gameOver(void);
void set_pixel_display(uint8_t x, uint8_t y);
void unset_pixel_display(uint8_t x, uint8_t y);
void display_screen_update(void);
void display_draw_rectfilled(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t col);
void display_draw_rect(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t col);

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128*32];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);
