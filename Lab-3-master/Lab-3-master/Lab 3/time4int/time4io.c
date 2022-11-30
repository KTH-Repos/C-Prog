#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void)
{
  int tempValue = PORTD;
  tempValue = tempValue >> 8;

  int mask = 0x0f;
  return mask & tempValue;
}

int getbtns(void) {
  int tempValue = PORTD;
  tempValue = tempValue >> 5;

  int mask = 0x7;
  return mask & tempValue;
}
