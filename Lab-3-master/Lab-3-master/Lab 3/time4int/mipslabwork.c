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

  int prime = 1234567;
  int mytime = 0x5957;
  int timeoutcount = 0;
  volatile int *trise = (volatile int *) 0xbf886100;
  volatile int *porte = (volatile int *) 0xbf886110;

  char textstring[] = "text, more text, and even more text!";

  void enable_interrupts( void ) {
      asm volatile("ei"); //Enables interrupts globally.
  }

  // Interrupt Service Routine /
  void user_isr( void )
  {
    if (IFS(0) & 0x100) {    //Interrupt from Timer2
      IFSCLR(0) = 0x100;     //Rensar flaggan
      timeoutcount++;        //Ökar timeoutcount med 1
    }

    if (IFS(0) & 0x8000) {    //Interrupt from SW3(INT3 - External Interrupt 3)
      *porte = *porte + 1;    //Ökar LEDSEN med 1
      //IFS(0) = 0;
      IFSCLR(0) = 0x8000;     //Rensar flaggan
    }

    if (timeoutcount == 10) { //Om timeoutcount är 10
      time2string ( textstring, mytime );
      display_string( 3, textstring );
      display_update();     
      tick ( &mytime );      //Ökar värdet på tiden
      timeoutcount = 0;      //timeoutcount till 0
      *porte = *porte + 1;   //Ökar LEDSEN med 1
    }
  }

  // Lab-specific initialization goes here */
  void labinit( void )
  {
  //*trise = *trise & 0xfff0; //output
  //*porte = 0x00;

  //TRISD |= 0xfe0; //INPUT
  //PORTD = 0x00;


    //Timer 2
    //(80000000/10)/256 = 31250 (FÅR PLATS I 16-BITAR)
    IEC(0) = 0x8100; //Timer 2 och External interrupt 3
    IPC(2) = 4;
    IPC(3) = 0x4000000;      //0100 0000 0000 0000 0000 0000 0000  

    //led 
    *trise = *trise & 0xfff0; //output
    *porte = 0x00;

    //switch
    TRISD |= 0xfe0; //INPUT 1 as input, 0 as output
    PORTD = 0x00; 

    //IEC(0) = 0x8000;
    //INT3(0) = /*INT3(0) |*/ 0x100; // Tillåt  
    ///IPC(3) = IPC(3) | 0x10;

    //IC2IE = 1;
    //T3IE = 1;
    
    //IEC(0) = (1 << 8); //Enable interrup
    //IPC(2) = 4; //Priority 2

    //IEC(3) = (1 << 15);
    //IPC(3) = (1 << 26);
    //IPC(3) = (1 << 27);
    //IPC(3) = (1 << 28);

    T2CON = 0x70;
    PR2 = (80000000 / 256) / 10;
    TMR2 = 0;
    T2CONSET = 0x8000; //STARTAR TIMERN
    enable_interrupts();
  }

  
  // This function is called repetitively from the main program */
  void labwork( void )
  {
    prime = nextprime ( prime );
    display_string ( 0, itoaconv( prime ) );
    display_update();
  }