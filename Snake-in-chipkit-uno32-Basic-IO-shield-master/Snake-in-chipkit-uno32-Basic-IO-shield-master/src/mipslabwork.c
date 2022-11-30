#include <stdint.h>   // Declarations of uint_32 and the like /
#include <pic32mx.h>  // Declarations of system-specific addresses etc /
#include "mipslab.h"  

void *stdin, *stdout, *stderr; //RAND

volatile int *trise = (volatile int *) 0xbf886100;
volatile int *porte = (volatile int *) 0xbf886110;

int x0, y0, x1, y1;                     //Start position of snake
int dir;                                //Start direction 0 == right
int foodOnScreen = 0;                   //Food for snake
int foodPosition[125][29];              //Position of food
int historySteps = 100;                 //Number of steps taken by snake
int playerHistoryX[100];                //Number of steps in x-dir
int playerHistoryY[100];                //Number of steps in y-dir
int score = 0;                          
int highscore = 0;
int hasPressed = 0;

// Interrupt Service Routine //
void user_isr( void ) {
  return;
 }

// Control the LEDs */
void ledSwitch( void ) {

  *trise &= ~0xff; //output
  *porte = 0x00;
  TRISD |= 0xfe0;
  PORTD = 0x00;
 }

/* Display screen when the game is over
   by showing score, high-score and an 
   option for running game again.
 */
void gameOver( void ) {
  if (score > highscore) {
    highscore = score;
  }
  display_font_clear();
  display_string(0, " Game Over!");
  char text[20]; 
  sprintf(text, "Score: %d", score);
  display_string(1, text);
  sprintf(text, "Hscore: %d", highscore);
  display_string(2, text);
	display_string(3, " Start-BTN4");
  display_font_update();
    while(1) {
      if (getbtns() & 0x4) {
        break;
      }
    }
  delay( 1000 );
}

/* Generate food for snake by randomly generating 
   numbers as position for food in the playing field.
*/
void spawnFood( void ) {
  foodOnScreen = 1;                                    //Food amount on screen
  int i;
  for (i = foodOnScreen; i > 0; i--) {
    int posx = (rand() % 100) + 6;
    int posy = (rand() % 22) + 4;
    if (foodPosition[posx][posy] == 1){
      i += 1;
    }
    else {
      set_pixel_display(posx, posy);
      foodPosition[posx][posy] = 1;
    } 
  }    
}

/* Increase score by one when snake eats food and
   turn on LEDs according to accumulated points 
*/
void eatFood( void ) {
  score += 1;
  foodOnScreen -= 1;
  *porte = *porte + 1;
 }

/* Detect food around head of snake. If a pixel
   is on, then there is food to be eaten. Yummy!  
*/
void foundFood( void ) {
  int i;
  int k;
  for (i = -2; i < 5; i++) {
    for (k = -2; k < 5; k++) {
      if (foodPosition[x0+i][y0+k] == 1)
      {
        foodPosition[x0+i][y0+k] = 0;
        unset_pixel_display(x0+i, y0+k);
        eatFood();
      }
    }
  }               
  display_screen_update();
}

/* Start game by drawing snake and wall
 */
void playInit( void ) {
  x0 = 64, y0 = 16, x1 = 68, y1 = 19;                 //Start position of snake in screen display
  dir = 0;                                            //Start = 0 ----> right
  *porte = 0x00;                                      //Turn off all LEDs
  score = 0;                                          //Make score zero
  foodOnScreen = 0;

  display_draw_rectfilled(1, 1, 127, 31, 0);          //Remove everything on sceen
  int i, j;
  for (i = 0; i < 125; i++) {
    for (j = 0; j < 29; j++) {
      foodPosition[i][j] = 0;
    }
  }
  int k;
  for (k = 0; k < 100; k++) {                         //Erase history of snake
    playerHistoryX[k] = 0;
    playerHistoryY[k] = 0;
  }
  display_draw_rect(1, 1, 127, 31, 1);                //Draw the wall 
  display_draw_rectfilled(x0, y0, x1, y1, 1);
  spawnFood();
  display_screen_update();
}

/* Update data in historysteps to make
   illusion of movement of snake. 
*/
void updatePlayerHistory() {
  int i;
  for (i = (historySteps - 1); i > 0; i--) {
    playerHistoryX[i] = playerHistoryX[i - 1];
    playerHistoryY[i] = playerHistoryY[i - 1];   
  }
}

/* Remove the pixels in the tail of snake so that
   tail moves with the rest of snake body
 */
 void removeOldTail() {
  int position = (score*2)+4;
  if (playerHistoryX[position] != 0 || playerHistoryY[position] != 0) {
    int i;
    for(i=0; i<4; i++){
      unset_pixel_display(playerHistoryX[position], playerHistoryY[position] + i);
      unset_pixel_display(playerHistoryX[position] + 1, playerHistoryY[position] + i);
      unset_pixel_display(playerHistoryX[position] + 2, playerHistoryY[position] + i);
      unset_pixel_display(playerHistoryX[position] + 3, playerHistoryY[position] + i);
    }
    display_screen_update(); 
  }
}

/* Draw food on playing field 
*/
void drawFood( void ) {
  int x, y;
  for (x = 0; x < 125; x++) {
    for (y = 0; y < 31; y++) {
      if (foodPosition[x][y] == 1){
        set_pixel_display(x, y);
      }
    }
  }
}

/* Draw snake on screen
 */
void drawPlayer( void ) {
  if (score == 0) {
    display_draw_rectfilled(x0, y0, x1, y1, 0); 
  }
  if (dir == 0) { //Right
    x0 += 1;
    x1 += 1;
  }
  else if (dir == 1) { //Left
    x0 -= 1;
    x1 -= 1; 
  }
  else if (dir == 2) { //Up
    y0 += 1;
    y1 += 1; 
  }
  else if (dir == 3) { //Down
    y0 -= 1;
    y1 -= 1; 
  }
  display_draw_rectfilled(x0, y0, x1, y1, 1);
  display_screen_update();

  playerHistoryX[0] = x0;
  playerHistoryY[0] = y0;
  updatePlayerHistory();
  removeOldTail();
}

/* Update the direction of the snake by the
   following encoding of directions--------
   0=RIGHT 1=LEFT 2=UP 3=DOWN--------------
 */
void updatePlayerDir( void ) {
  if (hasPressed == 1) {
    hasPressed = 0;
  }
  else if ((getbtns() & 0x2) && (hasPressed == 0)) { // Right
    if (dir == 0) {
      dir = 3;    
    }
    else if (dir == 1) {
      dir = 2;     
    }
    else if (dir == 2) {
      dir = 0;     
    }
    else if (dir == 3) {
      dir = 1;     
    }
    hasPressed = 1;
    }
  else if ((getbtns() & 0x1) && (hasPressed == 0)) { // Left
    if (dir == 0) {
      dir = 2;    
      }
      else if (dir == 1) {
        dir = 3;     
      }
      else if (dir == 2) {
        dir = 1;     
      }
      else if (dir == 3) {
        dir = 0;     
    }
    hasPressed = 1;
  }
}

/* Check if snake collides with the wall
 */
int checkCollisionWithWall( void ) {
  if (x0 == 2 || x1 == 126 || y0 == 2 || y1 == 30) {
    gameOver();
    return 1;
  }
  return 0;
}

/* Check if snake collides with itself
 */
int checkCollisionWithSnake ( void ) {
  int i;
  for (i = 8; i < (score*2)+4; i++) {
    if (playerHistoryX[i] != 0 && playerHistoryY[i] != 0 && score >= 1) {
      if (x0 == playerHistoryX[i] && y0 == playerHistoryY[i]) {
        gameOver();
        return 1;  
      }
    }
  }
  return 0;
}

/* 
 */
void runGame( void ) {
  while(1) {
    playInit();
    while(1) {
      drawPlayer();
      drawFood();
      if (checkCollisionWithWall() == 1) {
        break;
      }
      if (checkCollisionWithSnake() == 1) {
        break;
      }
      foundFood();
      if (foodOnScreen == 0) {
        spawnFood();
      }
      display_screen_update();
      delay(100 - score*3);
      updatePlayerDir();
    }
  }
}