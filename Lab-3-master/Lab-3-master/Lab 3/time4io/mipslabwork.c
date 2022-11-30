/*mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING */

#include <stdint.h>   // Declarations of uint_32 and the like /
#include <pic32mx.h>  // Declarations of system-specific addresses etc /
#include "mipslab.h"  // Declatations for these labs /

int mytime = 0x5957;
volatile int *trise = (volatile int *) 0xbf886100;
volatile int *porte = (volatile int *) 0xbf886110;


char textstring[] = "text, more text, and even more text!";

// Interrupt Service Routine /
void user_isr( void )
{
  return;
}

// Lab-specific initialization goes here */
void labinit( void )
{
  *trise &= ~0xff; //output

  *porte = 0x00;

  TRISD |= 0xfe0;
  PORTD = 0x00;
}

void swANDbtns(void) {
  if (((getbtns() >> 2) & 0x1) == 1) { //BTN 4 MED SWITCH 4 - FUNKAR HELT
    //if ((getsw() >> 3) == 1) {
      int temp = mytime & 0xfff;

      mytime = ((getsw() << 12) | temp);
    }
  //}
  if (((getbtns() >> 1) & 0x1) == 1) { //BTN 3 MED SWITCH 3 -
    //if (((getsw() >> 2) & 0x1) == 1) {
        int temp1 = mytime & 0xff;
        int temp2 = (mytime & 0xf000) >> 8;
        temp2 = temp2 >> 4;

        mytime = ((temp2 << 12) | (getsw() << 8) | temp1);
      }
   //}
   if ((getbtns() & 0x1) == 1) { //BTN 2 MED SWITCH 2 -
     //if (((getsw() >> 1) & 0x1) == 1) {
         int temp1 = mytime & 0xf;
         int temp2 = (mytime & 0xff00) >> 8;

         mytime = ((temp2 << 8) | (getsw() << 4) | temp1);
     }
   //}
}

// This function is called repetitively from the main program */
void labwork( void )
{
  delay( 1000 );
  time2string( textstring, mytime );
  display_string( 3, textstring );
  display_update();
  tick( &mytime );
  *porte = *porte + 1;
  display_image(96, icon);
  swANDbtns();
}
