#include "banks.h"

#define MYBANK BANK_2

#include "b2_commands.h"
#include "b0_main.h"
#include <vdp.h>

void handleExit() {
  resetF18A();
  __asm__("li r0,0\n\t"
          "blwp *r0\n\t"
          : : : "r0" );
}

