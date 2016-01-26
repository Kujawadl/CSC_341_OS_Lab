#include "binops.h"

#ifndef MACHINE
#define MACHINE

/* Define condition codes for hardware functions
	 > : 001
	 < : 100
	 = : 010
	>= : 011
	<= : 110
*/
#define GRT 1
#define LST 4
#define EQL 2
#define GEQ 3
#define LEQ 6

/* Define boolean type */
typedef enum { false, true } bool;

/* Define registers type */
typedef struct registers {
	short int r1, r2, r3, rA, IR, PC:8;
} registers;

/* Declare and initialize machine registers */
registers machine = {
	.r1 = 0,
	.r2 = 0,
	.r3 = 0,
	.rA = 0,
	.IR = 0,
	.PC = 0
};

/* Declare main memory */
short int main_memory[256];

/* Uses value in IR to determine course of action */
/* Returns false if errors */
bool interpreter();

/* Retrieves the opcode */
/* Returns an int 0-15 */
short int getOpcode(short int num);

/* Retrieves the address mode */
/* Returns 0 or 1 */
short int getAddrMode(short int num);

/* Retrieves the register code */
/* Returns an int 0-3 */
short int getRegCode(short int num);

/* Retrieves the operand */
/* Returns an int 0-255 */
short int getOperand(short int num);

bool LOD();
bool STO();
bool ADD();
bool SUB();
bool ADR();
bool SUR();
bool AND();
bool IOR();
bool NOT();
bool JMP();
bool JEQ();
bool JGT();
bool JLT();
bool CMP();
bool CLR();
bool HLT();

/* Condition codes unneeded outside hardware */
#undef GRT
#undef LST
#undef EQL
#undef GEQ
#undef LEQ

#endif