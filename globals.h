#ifndef _GLOBALS_H
#define _GLOBALS_H 1

#include "dsrutil.h"

extern struct DeviceServiceRoutine* currentDsr;
extern struct DeviceServiceRoutine* scriptDsr;
extern struct PAB* scriptPab;
extern char currentPath[256];
extern char filterglob[12];
extern int backspace;
extern unsigned int displayWidth;
extern int scripton;
extern int lineno;

#endif
