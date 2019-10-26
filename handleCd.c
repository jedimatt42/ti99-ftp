#include "strutil.h"
#include "parsing.h"
#include "globals.h"
#include "commands.h"
#include "terminal.h"
#include <string.h>

void handleCd() {
  struct DeviceServiceRoutine* dsr = 0;
  char path[256];
  parsePathParam(&dsr, path, PR_REQUIRED);
  if (dsr == 0) {
    tputs("no path: drive or folder specified\n");
    return;
  }
  if (path[strlen(path)-1] != '.') {
    strcat(path, ".");
  }
  unsigned int stat = existsDir(dsr, path);
  if (stat != 0) {
    tputs("error, device/folder not found: ");
    tputs(path);
    tputc('\n');
    return;
  }
  
  currentDsr = dsr;
  strcpy(currentPath, path);
}

