#include <stdio.h>
#include <string.h>
#include "machine.h"

#ifndef OS
#define OS

registers U1, U2, SYS;
U1.IR = 61440;
U2.IR = 61440;
/* 
Note: U1 & U2 instruction registers contain 61440 (HLT) 
while user is not running any program. The run function
will later assign the IR to the value of the starting
memory address of the program.
   
The scheduler can quickly determine if a user is running
something by testing the opcode of the IR for HLT (1111).
*/

/* Dump contents of main memory */
void dump(bool dumpRegs);
void dump() { dump(false); } /* default */

/* Round-robin scheduler */
void scheduler();

/* Main function (O/S Initialization) */
int main(int argc, char** argv);

#endif

