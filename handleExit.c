#include "commands.h"
#include "main.h"

void handleExit() {
  resetF18A();

  reboot();
}

