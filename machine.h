#ifndef MACHINE
#define MACHINE

/* Debugging flags */
#define DEBUG
#define DEBUG_VERBOSE
#undef DEBUG_VERBOSE
#undef DEBUG

/* If DEBUG_VERBOSE, then DEBUG */
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

/* Define addressing modes */
#define DIRECT 0
#define IMMEDIATE 1

/* Define boolean type */
typedef enum { false, true } bool;

/* Define machine registers type */
/* Machine register type also has a condition code register */
typedef struct {
	unsigned short int r1, r2, r3, rA, IR, PC:8;
	unsigned short int CR:3;
} registers;

/* Declare and initialize machine registers */
extern registers machine;

/* Declare main memory */
unsigned short int main_memory[256];

/* Uses value in IR to determine course of action */
/* Returns false if errors */
bool interpreter();

/* Retrieves the opcode */
/* Returns an int 0-15 */
unsigned short int getOpcode(unsigned short int num);

/* Retrieves the address mode */
/* Returns 0 or 1 */
unsigned short int getAddrMode(unsigned short int num);

/* Retrieves the register code */
/* Returns an int 0-3 */
unsigned short int getRegCode(unsigned short int num);

/* Retrieves the operand */
/* Returns an int 0-255 */
unsigned short int getOperand(unsigned short int num);

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

unsigned short int getCondCode(unsigned short int);
unsigned short int* getRegister();

#ifdef DEBUG
void printDebug(char*);
#endif

#endif
