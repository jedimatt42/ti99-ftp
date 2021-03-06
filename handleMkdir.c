#include "commands.h"
#include "terminal.h"
#include <string.h>
#include "strutil.h"
#include "globals.h"
#include "dsrutil.h"
#include "lvl2.h"

void handleMkdir() {
  char* dirname = strtok(0, " ");
  if (dirname == 0) {
    tputs("error, must specify a directory name\n");
    return;
  }

  unsigned int unit = path2unitmask(currentPath);

  lvl2_setdir(currentDsr->crubase, unit, currentPath);

  unsigned int err = lvl2_mkdir(currentDsr->crubase, unit, dirname);
  if (err) {
    tputs("cannot create directory ");
    tputs(currentPath);
    tputs(dirname);
    tputc('\n');
  }
}
