#include "parsing.h"
#include "main.h"
#include "globals.h"
#include "commands.h"
#include "dsrutil.h"
#include "strutil.h"
#include "terminal.h"
#include <string.h>

#define MATCH(x,y) (!(strcmpi(x,y)))

#define COMMAND(x, y) if (MATCH(tok, x)) y();

static int isAssignment(char* str) {
  int i = 0;
  while(str[i] != 0) {
    if (str[i] == '=') {
      return 1;
    }
    i++;
  }
  return 0;
}

int parsePath(char* path, char* devicename) {
  char workbuf[14];
  int crubase = 0;
  strncpy(workbuf, path, 14);
  char* tok = strtok(workbuf, ". ");
  if (tok != 0 && tok[0] == '1' && strlen(tok) == 4) {
    crubase = htoi(tok);
    tok = strtok(0, ". ");
    strcpy(devicename, tok);
  } else {
    strcpy(devicename, tok);
  }
  return crubase;
}

void parsePathParam(struct DeviceServiceRoutine** dsr, char* buffer, int requirements) {
  filterglob[0] = 0;
  buffer[0] = 0; // null terminate so later we can tell if it is prepared or not.
  char* path = strtok(0, " ");
  *dsr = currentDsr;
  if (path == 0) {
    if (requirements & PR_REQUIRED) {
      *dsr = 0; // set dsr pointer to null to indicate missing required parameter.
      return;
    }
    strcpy(buffer, currentPath); // if not required, use current path
    return;
  } else {
    char devicename[8];
    if (0 == strcmp("..", path)) {
      int ldot = lindexof(currentPath, '.', strlen(currentPath) - 2);
      if (ldot == -1) {
        *dsr = 0;
        tputs("No parent folder\n");
        return;
      }
      strncpy(buffer, currentPath, ldot + 1);
      return;
    } else {
      int crubase = parsePath(path, devicename);
      *dsr = findDsr(devicename, crubase);
      if (*dsr == 0) {
        // not a base device, so try subdir
        strcpy(buffer, currentPath);
        strcat(buffer, path);
        crubase = parsePath(buffer, devicename);
        *dsr = findDsr(devicename, crubase);
        // if still not found, then give up.
        if (*dsr == 0) {  
          tputs("device not found.\n");
          return;
        }
        // at this stage, buffer is set with result device name.
      }
      if (crubase != 0) {
        path = strtok(path, ".");
        path = strtok(0, " ");
        strcpy(buffer, path);
      } else {
        if (buffer[0] == 0) {
          strcpy(buffer, path);
        }
      }
    }
  }
  // Todo: test for existance and matching requirements
  
  // separate path and filter if wildcards are supported.
  if (requirements & PR_WILDCARD) {
    int len = strlen(buffer);
    int dotidx = lindexof(buffer, '.', len);
    if (indexof(buffer + dotidx, '*') != -1) {
      strcpy(filterglob, buffer+dotidx+1);
      buffer[dotidx] = 0;
    }
  }
}

// if filename matches current glob, or no glob is specified, return true.
int globMatches(char* filename) {
  if (filterglob[0] == 0) {
    return 1;
  }

  int prelen = indexof(filterglob, '*') + 1;
  char prefix[12];
  strncpy(prefix, filterglob, prelen);

  char suffix[12];
  strcpy(suffix, filterglob+prelen);

  return str_startswith(filename, prefix) && str_endswith(filename, suffix);
}
