#include "commands.h"

#include "main.h"
#include "strutil.h"
#include "terminal.h"
#include <string.h>

void handleWidth() {
  char* tok = strtok(0, " ");
  int width = atoi(tok);

  if (width == 80 && isF18A()) {
    setupScreen(80);
    return;
  }

  if (width == 40) {
    setupScreen(40);
    return;
  }
  
  tputs("no supported width specified\n");
}

