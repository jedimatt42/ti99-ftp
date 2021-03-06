#include "lvl2.h"
#include "dsrutil.h"
#include "mds_dsrlnk.h"
#include "strutil.h"
#include "terminal.h"

#include <string.h>
#include <vdp.h>

#define LVL2_STATUS *((volatile unsigned char*)0x8350)
#define LVL2_UNIT *((volatile unsigned char*)0x834C)
#define LVL2_PROTECT *((volatile unsigned char*)0x834D)
#define LVL2_PARAMADDR1 *((volatile unsigned int*)0x834E)
#define LVL2_PARAMADDR2 *((volatile unsigned int*)0x8350)

#define UNITNO(x) (x & 0x0F)
#define OPNAME(x,y) ((x & 0xF0)|(y & 0x0F))

// Returns lvl2 base code in left nibble, and unit number in right nibble
// 	Floppy disk controllers:	DSK	>1x	
//	Myarc harddisk controller:	WDS	>2x
//	Scuzzy controller		SCS	>2x
//	IDE controller:			IDE	>8x
//	Ti to PC serial connection:	HDX	>9x
unsigned int path2unitmask(char* currentPath) {
  unsigned int operationSet = 0x0010;
  char drive[9];
  strncpy(drive, currentPath, 9);
  int l = indexof(drive, '.');
  drive[l] = 0;
  if (str_equals("TIPI", drive)) {
    return 0x0010;
  }
  l--;
  unsigned char unit = drive[l] - '0' & 0x0F;

  drive[l] = 0;

  if (str_equals(drive, "WDS")) {
    operationSet = 0x0020;
  } else if (str_equals(drive, "SCS")) {
    operationSet = 0x0020; // yep, same as Myarc
  } else if (str_equals(drive, "IDE")) {
    operationSet = 0x0080;
  } else if (str_equals(drive, "HDX")) {
    operationSet = 0x0090;
  }

  return operationSet | unit;
}

unsigned int lvl2_protect(int crubase, int unit, char* filename, int protect) {
  return base_lvl2(crubase, unit, LVL2_OP_PROTECT, filename, 0, protect ? 0xff : 0x00);
}

unsigned int lvl2_setdir(int crubase, int unit, char* path) {
  LVL2_PARAMADDR1 = FBUF;
  int len = strlen(path);
  if (len > 39) {
    return 0xFE;
  }
  vdpchar(FBUF,(unsigned char) len);
  vdpmemcpy(FBUF+1, path, len);

  LVL2_UNIT = UNITNO(unit);
  LVL2_STATUS = 0;

  call_lvl2(crubase, OPNAME(unit, LVL2_OP_SETDIR));

  return LVL2_STATUS;
}

unsigned int lvl2_mkdir(int crubase, int unit, char* dirname) {
  return base_lvl2(crubase, unit, LVL2_OP_MKDIR, dirname, 0, 0);
}

unsigned int lvl2_rmdir(int crubase, int unit, char* dirname) {
  return base_lvl2(crubase, unit, LVL2_OP_DELDIR, dirname, 0, 0);
}

unsigned int lvl2_rename(int crubase, int unit, char* oldname, char* newname) {
  return base_lvl2(crubase, unit, LVL2_OP_RENAME, newname, oldname, 0);
}

unsigned int lvl2_rendir(int crubase, int unit, char* oldname, char* newname) {
  return base_lvl2(crubase, unit, LVL2_OP_RENDIR, newname, oldname, 0);
}

unsigned int lvl2_input(int crubase, int unit, char* filename, unsigned int blockcount, struct AddInfo* addInfoPtr) {
  return direct_io(crubase, unit, LVL2_OP_INPUT, filename, blockcount, addInfoPtr);
}

unsigned int lvl2_output(int crubase, int unit, char* filename, unsigned int blockcount, struct AddInfo* addInfoPtr) {
  return direct_io(crubase, unit, LVL2_OP_OUTPUT, filename, blockcount, addInfoPtr);
}

unsigned char direct_io(int crubase, char unit, char operation, char* filename, unsigned char blockcount, struct AddInfo* addInfoPtr) {
  LVL2_PARAMADDR1 = FBUF;
  strpad(filename, 10, ' ');
  vdpmemcpy(FBUF, filename, 10);

  LVL2_UNIT = UNITNO(unit);
  LVL2_PROTECT = blockcount;
  LVL2_STATUS = ((unsigned int) addInfoPtr) - 0x8300;

  addInfoPtr->buffer = VDPFBUF; // safe from file and path name overwrites.

  call_lvl2(crubase, OPNAME(unit, operation));

  return LVL2_STATUS;
}

// Setup parameters suitably for most lvl2 calls.
unsigned char __attribute__((noinline)) base_lvl2(int crubase, char unit, char operation, char* name1, char* name2, char param0) {
  LVL2_UNIT = UNITNO(unit);
  LVL2_PROTECT = param0;
  LVL2_PARAMADDR1 = FBUF;

  strpad(name1, 10, ' ');
  vdpmemcpy(LVL2_PARAMADDR1, name1, 10);

  if (name2 == 0) {
    LVL2_STATUS = 0;
  } else {
    LVL2_PARAMADDR2 = FBUF + 10;
    strpad(name2, 10, ' ');
    vdpmemcpy(LVL2_PARAMADDR2, name2, 10);
  }

  call_lvl2(crubase, OPNAME(unit, operation));

  return LVL2_STATUS;
}

unsigned int __attribute__((noinline)) subroutine(int crubase, unsigned char operation) {
  enableROM(crubase);
  unsigned int addr = 0;

  struct DeviceRomHeader* dsrrom = (struct DeviceRomHeader*) 0x4000;
  struct NameLink* entry = (struct NameLink*) dsrrom->basiclnk;
  while(entry != 0) {
    if (entry->name[0] == 1 && entry->name[1] == operation) {
      addr = entry->routine;
      break;
    }
    entry = entry->next;
  }
  disableROM(crubase);
  return addr;
}

void __attribute__((noinline)) call_lvl2(int crubase, unsigned char operation) {
  unsigned int addr = subroutine(crubase, operation);
  if (addr == 0) {
    LVL2_STATUS = 0xFF;
    return;
  }

  __asm__(
    	" mov %0,@>83F8		; prepare GPLWS r12 with crubase\n"
      " mov %1,@>83F2   ; set r9 to subroutine address\n"
    	"	lwpi 0x83e0     ; get gplws\n"
      " sbo 0           ; turn on card dsr\n"
      " bl *r9          ; call rubroutine\n"
      " nop             ; lvl2 routines never 'skip' request\n"
      " sbz 0           ; turn off card dsr\n"
      " lwpi 0x8300     ; assuming gcc workspace is here\n"
      :
      : "r" (crubase), "r" (addr)
      : "r12"
  );
}