/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

//void *stdin, *stdout, *stderr;
static uint8_t screen_content[128][4];

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */
  
  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
  DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}


void set_pixel_display(uint8_t x, uint8_t y)
{
    uint8_t bit_offset = y % 8;                 /* Offset is the remainder  */
    uint8_t byte_select = (y - bit_offset) / 8; /* Select is floor function */

    /* Check if valid coordinates */
    if(x < 0 || x > 127 || y < 0 || y > 31)
        return;

    /* Copy to screen */
    screen_content[x][byte_select] |= 0x1 << bit_offset;

    return;
}

void unset_pixel_display(uint8_t x, uint8_t y)
{
    uint8_t bit_offset = y % 8;                 /* Offset is the remainder  */
    uint8_t byte_select = (y - bit_offset) / 8; /* Select is floor function */

    /* Check if valid coordinates */
    if(x < 0 || x > 127 || y < 0 || y > 31)
        return;

    /* Copy to screen */
    screen_content[x][byte_select] &= ~(0x1 << bit_offset);

    return;
}

void display_font_clear(void)
{
  int i;
  for(i = 0; i < 16; i++) {
    textbuffer[0][i] = ' ';
    textbuffer[1][i] = ' ';
    textbuffer[2][i] = ' ';
    textbuffer[3][i] = ' ';
  }
}


void display_font_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

void display_screen_update(void) {
  uint8_t cur_page;
  uint8_t cur_col;
  uint8_t data_byte;

  for(cur_page = 0; cur_page < 4; cur_page++)
  {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    quicksleep(10);

    //page address
    spi_send_recv(0x22);
    spi_send_recv(cur_page);

    //Cursor at left column
    spi_send_recv(0x00);
    spi_send_recv(0x10);

    DISPLAY_CHANGE_TO_DATA_MODE;
    quicksleep(10);

    // Set each column in page to zero
    for(cur_col = 0; cur_col < 128; cur_col++)
      {
        data_byte = screen_content[cur_col][cur_page];
        spi_send_recv(data_byte);
      }
    }  
  }

void display_draw_rectfilled(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t col)
{
    uint8_t width = x1 - x0;
    uint8_t height = y1 - y0;
    uint8_t i, j;

    //Valid
    if(x1 < x0 || y1 < y0)
        return;

    //Draw
    for(i = x0; (i < (x0+width)) && (i < 128); i++)
    {
        for(j = y0; (j < (y0+height)) && (j < 32); j++)
        {
            if(col==1)
                set_pixel_display(i,j); 
            if(col==0)
                unset_pixel_display(i,j); 
        }
    }
}  

void display_draw_rect(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t col)
{
    uint8_t width = x1 - x0;
    uint8_t height = y1 - y0;
    uint8_t i, j;

    //Valid
    if(x1 < x0 || y1 < y0)
        return;

    //Draw
    for(i = x0; (i < (x0+width)) && (i < 128); i++)    
    {
        if(col==1)
        {
            set_pixel_display(i,y0);
            set_pixel_display(i,y0+height-1);
        }
        if(col==0)
        {
            unset_pixel_display(i,y0);
            unset_pixel_display(i,y0+height-1);            
        }
    }

    //Draw
    for(i = y0; (i < (y0+height)) && (i < 32); i++)
    {
        if(col==1)
        {
            set_pixel_display(x0,i);
            set_pixel_display(x0+width-1,i);
        }
        if(col==0)
        {
            unset_pixel_display(x0,i);
            unset_pixel_display(x0+width-1,i);          
        }        
    }
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}