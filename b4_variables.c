#include "banks.h"
#define MYBANK BANK_4

#include "b4_variables.h"
#include "b1cp_terminal.h"
#include "b1cp_strutil.h"
#include "string.h"

struct __attribute__((__packed__)) VarEntry {
  char name[MAX_VAR_NAME+1]; // 
  char value[MAX_VAR_VAL+1]; // value
};

#define VAR_COUNT 20

struct VarEntry var_map[VAR_COUNT];
static int vars_next = 0;

static int vars_find(char* name) {
  for(int i = 0; i < vars_next; i++) {
    if (0 == strcmpi(name, var_map[i].name)) {
      return i;
    }
  }
  return -1;
}

static void vars_add(char* name, char* value) {
  if (vars_next >= VAR_COUNT) {
    tputs("Error, no more room in variable dictionary\n");
    return;
  }
  strcpy(var_map[vars_next].name, name);
  strcpy(var_map[vars_next].value, value);
  vars_next++;
}

void vars_set(char* name, char* value) {
  if (strlen(name) > MAX_VAR_NAME) {
    tputs("Error, variable name too long, limit is ");
    tputs(uint2str(MAX_VAR_NAME));
    tputs("\n");
    return;
  }
  if (strlen(value) > MAX_VAR_VAL) {
    tputs("Error, variable value too long, limit is ");
    tputs(uint2str(MAX_VAR_VAL));
    tputs("\n");
    return;
  }
  int i = vars_find(name);
  if (i == -1) {
    vars_add(name, value);
  } else {
    strcpy(var_map[i].value, value);
  }
}

char* vars_get(char* name) {
  int i = vars_find(name);
  if (i != -1) {
    return var_map[i].value;
  }
  return (char*) -1;
}

void printVars() {
  for(int i=0; i<vars_next; i++) {
    tputs(var_map[i].name);
    tputc('=');
    tputs(var_map[i].value);
    tputc('\n');
  }
  tputc('\n');
}
