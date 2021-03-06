#ifndef _DSR_H
#define _DSR_H 1

#include <files.h>

#define VPAB 0x3200
#define FBUF 0x3000

// Casting rom locations to the next 3 structs should ease 
// reasoning about any code accessing the rom header and
// lists.

struct __attribute__((__packed__)) EntryLink {
  struct EntryLink* next;
  unsigned int* routine;
};

struct __attribute__((__packed__)) NameLink {
  struct NameLink* next;
  unsigned int routine;
  char name[8]; // length byte + [upto] 7 characters. 
};

struct __attribute__((__packed__)) DeviceRomHeader {
  unsigned char flag;
  unsigned char version;
  unsigned int reserved1;
  struct EntryLink* powerlnk;
  struct NameLink* userlnk;
  struct NameLink* dsrlnk;
  struct NameLink* basiclnk;
  struct EntryLink* interruptlnk;
  unsigned int* gromsomething;
};

// fun with 'drives'
struct __attribute__((__packed__)) DeviceServiceRoutine {
  char name[8];
  int crubase;
  unsigned int addr;
  char unit;
}; 

// A cache of dsr names and addresses.
extern struct DeviceServiceRoutine dsrList[40];

struct __attribute__((__packed__)) DirEntry {
  char name[11];
  int type;
  int sectors;
  int reclen;
};

struct __attribute__((__packed__)) VolInfo {
  char volname[11];
  int available;
  int total;
  struct DeviceServiceRoutine* dsr;
};

unsigned int dsr_open(struct DeviceServiceRoutine* dsr, struct PAB* pab, const char* fname, int flags, int reclen);
unsigned int dsr_close(struct DeviceServiceRoutine* dsr, struct PAB* pab);
unsigned int dsr_read(struct DeviceServiceRoutine* dsr, struct PAB* pab, int recordNumber);
unsigned int dsr_write(struct DeviceServiceRoutine* dsr, struct PAB* pab, unsigned char* record, int reclen);
unsigned int dsr_status(struct DeviceServiceRoutine* dsr, struct PAB* pab);
unsigned int dsr_delete(struct DeviceServiceRoutine* dsr, struct PAB* pab);
unsigned int dsr_ea5load(struct DeviceServiceRoutine* dsr, const char* fname);
unsigned int dsr_reset(struct DeviceServiceRoutine* dsr, struct PAB* pab, int record);

typedef void (*vol_entry_cb)(struct VolInfo*);
typedef void (*dir_entry_cb)(struct DirEntry*);

unsigned char loadDir(struct DeviceServiceRoutine* dsr, const char* pathname, vol_entry_cb vol_cb, dir_entry_cb dir_cb);
unsigned int existsDir(struct DeviceServiceRoutine* dsr, const char* pathname);
unsigned int existsFile(struct DeviceServiceRoutine* dsr, const char* pathname);

void loadDriveDSRs();

void enableROM(int crubase);
void disableROM(int crubase);
int isDrive(char* basicstr);

struct DeviceServiceRoutine* findDsr(char* devicename, int crubase);

void initPab(struct PAB* pab);

#endif
