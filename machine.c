#include "machine.h"

/* Initialize machine registers */
machine_registers machine = {0, 0, 0, 0, 0, 0, 0};

/* Uses value in IR to determine course of action */
/* Returns false if errors */
bool interpreter() {
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
short int getOpcode(short int num) {
	return (num & 61440)>>12;
}

/* Retrieves the address mode */
/* Returns 0 or 1 */
short int getAddrMode(short int num) {
	return (num & 2048)>>11;
}

/* Retrieves the register code */
/* Returns an int 0-3 */
short int getRegCode(short int num) {
	return (num & 1792)>>8;
}

/* Retrieves the operand */
/* Returns an int 0-255 */
short int getOperand(short int num) {
	return (num & 255);
}

/* Loads a value into a register */
bool LOD() {
	short int addr = getAddrMode(machine.IR);
	short int operand = getOperand(machine.IR);
	short int *reg = getRegister(getRegCode(machine.IR));

	*reg = (addr == DIRECT ? main_memory[operand] : operand);

	return true;
}

/* Stores a value from a register to a memory location */
bool STO() {
	short int *reg = getRegister(getRegCode(machine.IR));
	main_memory[getOperand(machine.IR)] = *reg;
	return true;
}

/* Adds the input to rA */
/* TODO: Verify overflow checking */
bool ADD() {
	short int operand = getOperand(machine.IR);
	short int addr = getAddrMode(machine.IR);
	int over = (int)machine.rA;
	machine.rA += (addr == DIRECT ? main_memory[operand] : operand);
	over += (int)(addr == DIRECT ? main_memory[operand] : operand);

	/* If overflow occurs, return false */
	if (over == (int)machine.rA) {
		machine.CR = getCondCode(machine.rA)
		return true;
	} else {
		return false;
	}
}

/* Subtracts the input from rA */
/* TODO: Verify overflow checking */
bool SUB() {
	short int addr = getAddrMode(machine.IR);
	short int operand = getOperand(machine.IR);
	int over = (int)machine.rA;

	machine.rA -= (addr == DIRECT ? main_memory[operand] : operand);
	over -= (int)(addr == DIRECT ? main_memory[operand] : operand);

	/* If overflow occurs, return false */
	if (over == (int)machine.rA) {
		machine.CR = getCondCode(machine.rA)
		return true;
	} else {
		return false;
	}
}

/* Adds the input to the specified register */
/* TODO: Verify overflow checking */
bool ADR() {
	short int *reg = getRegister(getRegCode(machine.IR));
	short int addr = getAddrMode(machine.IR);
	short int operand = getOperand(machine.IR);
	int over = (int)*reg;

	*reg += (addr == DIRECT ? main_memory[operand] : operand);
	over += (int)(addr == DIRECT ? main_memory[operand] : operand);

	/* Effect condition code; return false if overflow */
	if (over == (int)*reg) {
		machine.CR = getCondCode(*reg);
		return true;
	} else {
		return false
	}
}

/* Subtracts the input from the specified register */
/* TODO: Verify overflow checking */
bool SUR() {
	short int *reg = getRegister(getRegCode(machine.IR));
	short int addr = getAddrMode(machine.IR);
	short int operand = getOperand(machine.IR);
	int over = (int)*reg;

	*reg -= (addr == DIRECT ? main_memory[operand] : operand);
	over -= (int)(addr == DIRECT ? main_memory[operand] : operand);

	/* Effect condition code; return false if overflow */
	if (over == (int)*reg) {
		machine.CR = getCondCode(*reg);
		return true;
	} else {
		return false
	}
}

/* The And fuction will do the bitwise operation AND using the specified */
/* register, and the address of a value provided in the arguments */
bool AND() {
	short int *reg = getRegister(getRegCode(machine.IR));
	short int operand =  main_memory[getOperand(machine.IR)];
	short int result = (*reg & operand);

	*reg = result; // set the new result into the register specified

	machine.CR = getCondCode(*reg);

	return true;
}
bool IOR() {return false;}

/* This is the bitwise NOT, or compliment function. This performs logical */
/* negation on each bit, forming the ones' complement of the given binary */
/* value. Bits that are 0 become 1, and those that are 1 become 0. */
bool NOT() {

	short int *reg = getRegister(getRegCode(machine.IR));
	*reg = ~*reg;

	machine.CR = getCondCode(*reg);

	return true;
}

/* Unconditional Jump */
/* Direct addressing is allowed, provided the value in the memory location
	is actually only using 8 bits. If the 8 highest-order bits contain anything,
	that would be a reference to a memory location that does not exist (PC: 8bits)
	and thus the JMP would return an error (essentially OutOfBounds) */
bool JMP() {
	short int addr = getAddrMode(machine.IR)
	short int jmpTo;
	if (addr == DIRECT) {
		jmpTo = main_memory[getOperand(machine.IR)];
		/* Check high-order bits */
		if (jmpTo & 65280 != 0)
			return false;
	} else if (addr == IMMEDIATE) {
		jmpTo = getOperand(machine.IR);
	} else {
		return false;
	}

	/* Set program counter to new address */
	machine.PC = jmpTo;
	return true;
}

/* Check condition flag register; jump if appropriate */
bool JEQ() {
	if (machine.CR == EQL)
		machine.PC = getOperand(machine.IR);
	return true;
}

/* Check condition flag register; jump if appropriate */
bool JGT() {
	if (machine.CR == GRT)
		machine.PC = getOperand(machine.IR);
	return true;
}

/* Check condition flag register; jump if appropriate */
bool JLT() {
	if (machine.CR == LST)
		machine.PC = getOperand(machine.IR);
	return true;
}

/* This is the compare function, it compares a */
/* direct or indirect value to the specified   */
/* register, it will always be register oriented */
/* I.E. It's always of the style of (reg < instruction) */
bool CMP() {

	short int *reg = getRegister(getRegCode(machine.IR));
	short int operand = getOperand(machine.IR);
	short int addr = getAddrMode(machine.IR);

	if (addr == DIRECT) {
		if (*reg < main_memory[operand]) {
			machine.CR = LST;
		} else if (*reg == main_memory[operand]) {
			machine.CR = EQL;
		} else if (*reg > main_memory[operand]) {
			machine.CR = GRT;
		}
	} else if (addr == IMMEDIATE) {
		if (*reg < operand) {
			machine.CR = LST;
		} else if (*reg == operand) {
			machine.CR = EQL;
		} else if (*reg > operand) {
			machine.CR = GRT;
		}
	}

	return true;
}
<<<<<<< Updated upstream

/* Will clear the specified regiester with 0s */
bool CLR() {
	short int *reg = getRegister(getRegCode(machine.IR));
	*reg = 0;
	machine.CR = 0;
	return true;
}
bool HLT() {return false;}
=======
bool CLR() {return false;}
bool HLT(bool exit_clean) {return exit_clean;}
>>>>>>> Stashed changes

/* Determine if a resultant value is positive, negative, or 0 */
/* Takes a short integer, returns a condition code (short integer) */
short int getCondCode(short int x) {
	if (x == 0)
		return EQL;
	else if (x > 0)
		return GRT;
	else if (x < 0)
		return LST;
}

/* Take a register code and return a pointer to that register */
short int* getRegister(short int regCode) {
	switch (regCode) {
		case 0: return &machine.rA; break;
		case 1: return &machine.r1; break;
		case 2: return &machine.r2; break;
		case 3: return &machine.r3; break;
		case default: HLT(false); break;
	}
}
