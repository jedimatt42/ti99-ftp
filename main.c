#include "main.h"

#include "getstr.h"
#include "parsing.h"
#include "globals.h"
#include "strutil.h"
#include "dsrutil.h"
#include "oem.h"
#include <sound.h>
#include <string.h>
#include <vdp.h>
#include <conio.h>
#include <kscan.h>
#include "terminal.h"
#include "commands.h"

#define APP_VER "1.0"

char commandbuf[256];

int isF18A() {
  unlock_f18a();
  unsigned char testcode[6] = { 0x04, 0xE0, 0x3F, 0x00, 0x03, 0x40 };
  vdpmemcpy(0x3F00, testcode, 6);
  {
    VDP_SET_REGISTER(0x36, 0x3F);
    VDP_SET_REGISTER(0x37, 0x00);
  }

  int frames = 3;
  while(frames--) {
    VDP_SET_ADDRESS(0x3F00);
    int res = VDPRD;
    if (!res) {
      return 1;
    }
  }
  
  return 0;
}

void resetF18A() {
  lock_f18a();
}

void setupScreen(int width) {
  resetF18A();
  if (width == 80) {
    displayWidth = 80;
    set_text80_color();
  } else { // 40 is the only other allowed value.
    displayWidth = 40;
    set_text();
  }
  initTerminal();
  termWidth = displayWidth;

  if (termWidth == 80) {
    bgcolor(background);
    textcolor(foreground);
    VDP_SET_REGISTER(VDP_REG_COL, background & 0x0f);
  } else {
    VDP_SET_REGISTER(VDP_REG_COL, foreground << 4 | background);
  }

  clrscr();
  gotoxy(0,0);
  defineChars();
}

void titleScreen() {
  tputs("Force FTP v");
  tputs(APP_VER);
  tputc(' ');
  tputs(__DATE__);
  tputs("\nwww.jedimatt42.com\n\n");
}

void main()
{
  MUTE_SOUND();

  foreground = 15;
  background = 4;
  setupScreen(isF18A() ? 80 : 40);

  loadDriveDSRs();
  titleScreen();

  handleFtp();
}
