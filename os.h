#include <stdio.h>
#include <string.h>
#include "machine.h"

#ifndef OS
#define OS

/* Dump contents of main memory */
void dump(bool dumpRegs);
void dump() { dump(false); } /* default */

#endif