#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine.h"

#ifndef OS
#define OS

user_registers U1 = {0, 0, 0, 0, 61440, 0};
user_registers U2 = {0, 0, 0, 0, 61440, 0};
user_registers SYS;
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

/* Round-robin scheduler */
void scheduler();

/* Main function (O/S Initialization) */
int main(int argc, char** argv);

void readFile();

#endif
