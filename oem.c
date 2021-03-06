#include "oem.h"
#include <vdp.h>

void rom2vdp(int pAddr, char* pSrc, int cnt);

void defineChars() {
  charsetlc();
  rom2vdp(gPattern, &PAT0, 32 * 8);
  rom2vdp(gPattern + (127 * 8), &PAT127, 129 * 8);
}

void rom2vdp(int pAddr, char* pSrc, int cnt) {
  VDP_SET_ADDRESS_WRITE(pAddr);
  while (cnt--) {
    VDPWD=*(pSrc++);
  }
}

