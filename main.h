#ifndef _MAIN_H
#define _MAIN_H 1

#include "dsrutil.h"

void initGlobals();
void main();
int isF18A();
void resetF18A();
void setupScreen(int width);
void sleep(int jiffies);
void titleScreen();

inline void reboot() {
  __asm__(
    "blwp @>0000"
  );
}

#endif