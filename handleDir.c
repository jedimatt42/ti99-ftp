#include "commands.h"


#include "globals.h"
#include "parsing.h"
#include "strutil.h"
#include "dsrutil.h"
#include "terminal.h"
#include <string.h>

void onLongVolInfo(struct VolInfo* volInfo);
void onLongDirEntry(struct DirEntry* dirEntry);

void onWideVolInfo(struct VolInfo* volInfo);
void onWideDirEntry(struct DirEntry* dirEntry);

static int col = 0;

/*
 want to be able to handle:
   dir 1100.TIPI.GAMES
   dir
   dir F*
   dir TIPI.GAMES.P*
*/
void handleDir() {
  struct DeviceServiceRoutine* dsr = 0;

  char* peek = strtokpeek(0, " ");
  int wideFormat = 0 == strcmpi("/W", peek);
  if (wideFormat) {
    strtok(0, " "); // consume the option token.
  }

  char path[256];
  parsePathParam(&dsr, path, PR_OPTIONAL | PR_WILDCARD);
  if (dsr == 0) {
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

  if (wideFormat) {
    col = 0;
    loadDir(dsr, path, onWideVolInfo, onWideDirEntry);
    tputc('\n');
  } else {
    loadDir(dsr, path, onLongVolInfo, onLongDirEntry);
  }
  tputc('\n');
}

void onLongVolInfo(struct VolInfo* volInfo) {
  tputs("Diskname: ");
  tputs(volInfo->volname);
  if (displayWidth == 40) {
    tputc('\n');
  } else {
    tputc(' ');
  }
  tputs("Available: ");
  tputs(uint2str(volInfo->available));
  tputs(" Used: ");
  tputs(uint2str(volInfo->total - volInfo->available));
  tputs("\n\n");
  tputs("Name       Type    P Reclen Sectors\n");
  tputs("-----------------------------------\n");
}

const char* file_types[] = {
  "DIS/FIX",
  "DIS/VAR",
  "INT/FIX",
  "INT/VAR",
  "PROGRAM",
  "DIR"
};

void onLongDirEntry(struct DirEntry* dirEntry) {
  if (!globMatches(dirEntry->name)) {
    return;
  }
  tputs(dirEntry->name);
  cputpad(11, dirEntry->name);

  int de_type = (0x0007 & dirEntry->type) - 1;

  char* ftype = (char*) file_types[de_type];
  
  tputs(ftype);
  cputpad(8, ftype);

  if (dirEntry->type < 0) {
    tputs("P ");
  } else {
    tputs("  ");
  }

  if (de_type >= 5) { // is program or dir? skip record details.
    cputpad(7, "");
  } else {
    char* sizestr = uint2str(dirEntry->reclen);
    tputs(sizestr);
    cputpad(7, sizestr);
  }

  tputs(uint2str(dirEntry->sectors));

  tputc('\n');
}

void onWideVolInfo(struct VolInfo* volInfo) {
  // do nothing, we don't want volume info in wide listing
}

void onWideDirEntry(struct DirEntry* dirEntry) {
  if (!globMatches(dirEntry->name)) {
    return;
  }
  int collimit = displayWidth / 11;
  tputs(dirEntry->name);
  col++;
  if (col < collimit) {
    cputpad(11, dirEntry->name);
  } else {
    col = 0;
    tputc('\n');
  }
}
