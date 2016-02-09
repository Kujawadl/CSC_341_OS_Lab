#include "machine.h"

/* Initialize machine registers */
registers machine = {0, 0, 0, 0, 0, 0};

/* Uses value in IR to determine course of action */
/* Returns false if errors */
bool interpreter()
{
	machine.PC++; /* Increment Program Counter */
	short int op = getOpcode(machine.IR);
	switch (op) {
		case 0:  return LOD(); break;
		case 1:  return STO(); break;
		case 2:  return ADD(); break;
		case 3:  return SUB(); break;
		case 4:  return ADR(); break;
		case 5:  return SUR(); break;
		case 6:  return AND(); break;
		case 7:  return IOR(); break;
		case 8:  return NOT(); break;
		case 9:  return JMP(); break;
		case 10: return JEQ(); break;
		case 11: return JGT(); break;
		case 12: return JLT(); break;
		case 13: return CMP(); break;
		case 14: return CLR(); break;
		case 15: return HLT(); break;
		default: return false; break;
	}
}

/* Retrieves the opcode */
/* Returns an int 0-15 */
short int getOpcode(short int num)
{
	return (num & 61440)>>12;
}

/* Retrieves the address mode */
/* Returns 0 or 1 */
short int getAddrMode(short int num)
{
	return (num & 2048)>>11;
}

/* Retrieves the register code */
/* Returns an int 0-3 */
short int getRegCode(short int num)
{
	return (num & 1792)>>8;
}

/* Retrieves the operand */
/* Returns an int 0-255 */
short int getOperand(short int num)
{
	return (num & 255);
}

/* Loads a value into a register */
bool LOD()
{
	short int addr = getAddrMode(machine.IR);
	short int operand = getOperand(machine.IR);
	short int reg = getRegCode(machine.IR);

	if (addr == 0) {
		switch (reg) {
			case 0: machine.rA = getOperand(machine.IR); break;
			case 1: machine.r1 = getOperand(machine.IR); break;
			case 2: machine.r2 = getOperand(machine.IR); break;
			case 3: machine.r3 = getOperand(machine.IR); break;
			default: return false; break;
		};
	} else if (addr == 1) {
		switch (reg) {
			case 0: machine.rA = main_memory[getOperand(machine.IR)]; break;
			case 1: machine.r1 = main_memory[getOperand(machine.IR)]; break;
			case 2: machine.r2 = main_memory[getOperand(machine.IR)]; break;
			case 3: machine.r3 = main_memory[getOperand(machine.IR)]; break;
			default: return false; break;
		};
	} else {
		return false;
	}
	return true;
}

/* Stores a value from a register to a memory location */
bool STO()
{
	short int reg = getRegCode(machine.IR);
	switch (reg) {
		case 0: main_memory[getOperand(machine.IR)] = machine.rA; break;
		case 1: main_memory[getOperand(machine.IR)] = machine.r1; break;
		case 2: main_memory[getOperand(machine.IR)] = machine.r2; break;
		case 3: main_memory[getOperand(machine.IR)] = machine.r3; break;
		default: return false; break;
	};
	return true;
}

/* Adds the input to rA */
/* TODO: Verify overflow checking */
bool ADD()
{
	short int operand = getOperand(machine.IR);
	short int addr = getAddrMode(machine.IR);
	int over = (int)machine.rA;
	machine.rA += (addr == 0 ? operand : main_memory[operand]);
	over += (int)(addr == 0 ? operand : main_memory[operand]);

	/* If overflow occurs, return false */
	if (over == (int)machine.rA)
		return true;
	else
		return false;
}

/* Subtracts the input from rA */
/* TODO: Verify overflow checking */
bool SUB()
{
	short int operand = getOperand(machine.IR);
	short int addr = getAddrMode(machine.IR);
	int over = (int)machine.rA;
	machine.rA -= (addr == 0 ? operand : main_memory[operand]);
	over -= (int)(addr == 0 ? operand : main_memory[operand]);

	/* If overflow occurs, return false */
	if (over == (int)machine.rA)
		return true;
	else
		return false;
}

/* Adds the contents of the specified register to rA */
/* TODO: Verify overflow checking */
bool ADR()
{
	short int reg = getOperand(machine.IR);
	int over = (int)machine.rA;
	switch (reg) {
		case 1:
			machine.rA += machine.r1;
			over += (int)machine.r1;
			break;
		case 2:
			machine.rA += machine.r2;
			over += (int)machine.r2;
			break;
		case 3:
			machine.rA += machine.r3;
			over += (int)machine.r3;
			break;
		default:
			return false;
			break;
	}

	/* If overflow occurs, return false */
	if (over == (int)machine.rA)
		return true;
	else
		return false;
}

/* Subtracts the contents of the specified register from rA */
/* TODO: Verify overflow checking */
bool SUR()
{
	short int reg = getOperand(machine.IR);
	int over = (int)machine.rA;
	switch (reg) {
		case 1:
			machine.rA -= machine.r1;
			over -= (int)machine.r1;
			break;
		case 2:
			machine.rA -= machine.r2;
			over -= (int)machine.r2;
			break;
		case 3:
			machine.rA -= machine.r3;
			over -= (int)machine.r3;
			break;
		default:
			return false;
			break;
	}

	/* If overflow occurs, return false */
	if (over == (int)machine.rA)
		return true;
	else
		return false;
}

bool AND() {return false;}
bool IOR() {return false;}

/* This is the bitwise NOT, or compliment function. This performs logical */
/* negation on each bit, forming the ones' complement of the given binary */
/* value. Bits that are 0 become 1, and those that are 1 become 0. */
bool NOT() {

	short int operand = getOperand(machine.IR);
	short int reg = getRegCode(machine.IR);
	switch (reg) {
		case 1:
			machine.rA = ~machine.rA;
			break;
		case 1:
			machine.r1 = ~machine.r1;
			break;
		case 2:
			machine.r2 = ~machine.r2;
			break;
		case 3:
			machine.r3 = ~machine.r3;
			break;
		default:
			return false;
			break;
	}

	return true;
}

/* Unconditional Jump */
bool JMP()
{
	machine.PC = getOperand(machine.IR);
	return true;
}

/* Jump if rA is equal to the specified register */
bool JEQ()
{
	short int operand;
	switch (getRegCode(machine.IR)) {
		case 1: operand = machine.r1; break;
		case 2: operand = machine.r2; break;
		case 3: operand = machine.r3; break;
		default: return false; break;
	}
	if (machine.rA == operand)
		machine.PC = getOperand(machine.IR);
	return true;
}

/* Jump if rA is greater than the specified register */
/* TODO: Verify that the above statement is correct,
	i.e. if the comparison should be rA < r? instead. */
bool JGT()
{
	short int operand;
	switch (getRegCode(machine.IR)) {
		case 1: operand = machine.r1; break;
		case 2: operand = machine.r2; break;
		case 3: operand = machine.r3; break;
		default: return false; break;
	}
	if (machine.rA > operand)
		machine.PC = getOperand(machine.IR);
	return true;
}

/* Jump if rA is less than the specified register */
/* TODO: Verify that the above statement is correct,
	i.e. if the comparison should be rA > r? instead. */
bool JLT()
{
	short int operand;
	switch (getRegCode(machine.IR)) {
		case 1: operand = machine.r1; break;
		case 2: operand = machine.r2; break;
		case 3: operand = machine.r3; break;
		default: return false; break;
	}
	if (machine.rA < operand)
		machine.PC = getOperand(machine.IR);
	return true;
}
bool CMP() {return false;}
bool CLR() {return false;}
bool HLT() {return false;}
