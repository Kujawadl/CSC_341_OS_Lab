/*
 * machine.hpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341
 * Spring 2016
 */

#ifndef MACHINE
#define MACHINE

#include <string>		// string
#include <iostream> // cout, cerr
#include <iomanip>	// boolalpha, setw, setfill, dec, hex, endl
#include <unistd.h> // usleep
#include "utils.hpp"
	using namespace std;

#define normal "\033[0m"
#define red "\033[31m"
#define green "\033[32m"
#define blue "\033[34m"


// Debugging flags
#define DEBUG
#define DEBUG_VERBOSE
#undef DEBUG_VERBOSE
// #undef DEBUG

// If DEBUG_VERBOSE, then DEBUG
#ifdef DEBUG_VERBOSE
	#ifndef DEBUG
		#define DEBUG
	#endif
#endif

/* Define condition codes for hardware functions
> : 001
< : 100
= : 010
!= : 101
>= : 011
<= : 110
*/
#define GRT 1
#define LST 4
#define EQL 2
#define NEQ 5
#define GEQ 3
#define LEQ 6

// Define addressing modes
#define DIRECT 0
#define IMMEDIATE 1

#define QUANTUM 3

// Define machine registers type
// Machine register type also has a condition code register
struct registers{
	unsigned short int r1, r2, r3, rA, IR, PC:8;
	unsigned short int CR:3;
	PageTable* PTBR;

	registers(int pR1, int pR2, int pR3, int pRA, int pIR, int pPC, int pCR) :
		r1(pR1), r2(pR2), r3(pR3), rA(pRA), IR(pIR), PC(pPC), CR(pCR), PTBR(NULL) {}
};

extern int sysclock;
extern int timer_interrupt;

// Declare machine registers
extern registers machine;

// Declare main memory and disk
extern unsigned short int main_memory[256];
extern unsigned short int disk[512];

// Memory Management Unit
// Takes a logical address and converts it to a physical address
// by looking up frame numbers in the table store in PTBR
unsigned short int MMU(unsigned short int logicalAddress);

// Uses value in IR to determine course of action
// Returns false if errors
bool interpreter();

// Retrieves the opcode
// Returns an int 0-15
unsigned short int getOpcode(unsigned short int num);

// Retrieves the address mode
// Returns 0 or 1
unsigned short int getAddrMode(unsigned short int num);

// Retrieves the register code
// Returns an int 0-3
unsigned short int getRegCode(unsigned short int num);

// Retrieves the operand
// Returns an int 0-255
unsigned short int getOperand(unsigned short int num);

// Opcode functions
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

// Determines if a result is >, <, = (or +, -, 0)
unsigned short int getCondCode(unsigned short int);

// Take a register code and return a pointer to that register
unsigned short int* getRegister();

#ifdef DEBUG
void printDebug(string);
#endif

#endif
