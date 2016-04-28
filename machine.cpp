/*
 * machine.cpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341
 * Spring 2016
 */

#include "machine.hpp"

int *sysclock;          // Keep track of absolute time
int timer_interrupt;  // Send interrupt every QUANTUM ticks

// Initialize machine registers
registers machine = registers(0, 0, 0, 0, 0, 0, 0);

// Declare main memory
unsigned short int main_memory[256];


// Memory Management Unit
// Takes a logical address and converts it to a physical address
// by looking up frame numbers in the table store in PTBR
// Throws out_of_range exception if PTBR is null
unsigned short int MMU(unsigned short int logicalAddress) {
  if (!machine.PTBR) { // If PTBR pointer is null, throw an exception
    throw std::out_of_range("Page table base register is not initialized!");
  }

  unsigned short int page = (logicalAddress&252)>>2; // Returns 0-63
  unsigned short int offset = logicalAddress & 3; // Returns 0-3

  #ifdef DEBUG_VERBOSE
  cout << "Attempting to translate logical address " << logicalAddress
       << " (p#" << page << " w#" << offset << ")" << endl;
  #endif

  // Dereferenced PTBR pointer = PageTable object
  // operator[] either returns a fram number (if one is allocated to the page)
  // or allocates an empty frame to the page, then returns that frame number
  unsigned short int frame = (*machine.PTBR)[page];

  #ifdef DEBUG_VERBOSE
  cout << "Found physical address " << (frame<<2) + offset << " (f#" << frame
       << " w#" << offset << ")" << endl;
  #endif

  return ((frame<<2) + offset); // Bitshift frame# to the frame offset position.
                                // Offset remains the same
}

// Uses value in IR to determine course of action
// Returns false if errors
bool interpreter() {
  bool success = true;
  //While no error flag and no timer interrupt
  while (success && timer_interrupt < QUANTUM) {
    machine.IR = main_memory[MMU(machine.PC)];
    machine.PC++; // Increment Program Counter
    unsigned short int op = getOpcode(machine.IR);
    switch (op) {
      case 0:  success = LOD(); break;
      case 1:  success = STO(); break;
      case 2:  success = ADD(); break;
      case 3:  success = SUB(); break;
      case 4:  success = ADR(); break;
      case 5:  success = SUR(); break;
      case 6:  success = AND(); break;
      case 7:  success = IOR(); break;
      case 8:  success = NOT(); break;
      case 9:  success = JMP(); break;
      case 10: success = JEQ(); break;
      case 11: success = JGT(); break;
      case 12: success = JLT(); break;
      case 13: success = CMP(); break;
      case 14: success = CLR(); break;
      case 15:    return HLT(); break; //Quit early on HLT
      default: success = false; break;
    }
    usleep(1000000); // Sleep 1 second to allow easier instruction tracing
    (*sysclock)++;
    timer_interrupt++;
  }
  timer_interrupt = 0;
  return success;
}

// Retrieves the opcode
// Returns an int 0-15
unsigned short int getOpcode(unsigned short int num) {
  unsigned short int ret = (num & 61440)>>12;

  #ifdef DEBUG_VERBOSE
  cout << "In getOpCode(). IR: " << machine.IR << ", Opcode: " << ret << endl;
  #endif

  return ret;
}

// Retrieves the address mode
// Returns 0 or 1
unsigned short int getAddrMode(unsigned short int num) {
  unsigned short int ret = (num & 2048)>>11;

  #ifdef DEBUG_VERBOSE
  cout << "In getAddrMode(). IR: " << machine.IR << ", Addr: " << ret << endl;
  #endif

  return ret;
}

// Retrieves the register code
// Returns an int 0-3
unsigned short int getRegCode(unsigned short int num) {
  unsigned short int ret = (num & 1792)>>8;

  #ifdef DEBUG_VERBOSE
  cout << "In getRegCode(). IR: " << machine.IR << ", Regcode: " << ret << endl;
  #endif

  return ret;
}

// Retrieves the operand
// Returns an int 0-255
unsigned short int getOperand(unsigned short int num) {
  unsigned short int ret = (num & 255);

  #ifdef DEBUG_VERBOSE
  cout << "In getOperand(). IR: " << machine.IR << ", Operand: " << ret << endl;
  #endif

  return ret;
}

// Loads a value into a register
bool LOD() {
  unsigned short int addr = getAddrMode(machine.IR);
  unsigned short int operand = getOperand(machine.IR);
  unsigned short int *reg = getRegister();

  *reg = (addr == DIRECT ? main_memory[MMU(operand)] : operand);

  #ifdef DEBUG
  printDebug("LOD");
  #endif
  return true;
}

// Stores a value from a register to a memory location
bool STO() {
  unsigned short int operand = getOperand(machine.IR);
  unsigned short int *reg = getRegister();
  main_memory[MMU(operand)] = *reg;

  #ifdef DEBUG
  printDebug("STO");
  #endif
  return true;
}

// Adds the input to rA
// TODO: Verify overflow checking
bool ADD() {
  unsigned short int operand = getOperand(machine.IR);
  unsigned short int addr = getAddrMode(machine.IR);
  int over = (int)machine.rA;
  machine.rA += (addr == DIRECT ? main_memory[MMU(operand)] : operand);
  over += (int)(addr == DIRECT ? main_memory[MMU(operand)] : operand);

  #ifdef DEBUG
  printDebug("ADD");
  #endif

  // If overflow occurs, return false
  if (over == (int)machine.rA) {
    machine.CR = getCondCode(machine.rA);
    return true;
  } else {
    #ifdef DEBUG
    cerr << red;
    cerr << "!! OVERFLOW IN ADD()" << endl;
    cerr << normal;
    #endif
    return false;
  }
}

// Subtracts the input from rA
// TODO: Verify overflow checking
bool SUB() {
  unsigned short int addr = getAddrMode(machine.IR);
  unsigned short int operand = getOperand(machine.IR);
  int over = (int)machine.rA;

  machine.rA -= (addr == DIRECT ? main_memory[MMU(operand)] : operand);
  over -= (int)(addr == DIRECT ? main_memory[MMU(operand)] : operand);

  #ifdef DEBUG
  printDebug("SUB");
  #endif

  // If overflow occurs, return false
  if (over == (int)machine.rA) {
    machine.CR = getCondCode(machine.rA);
    return true;
  } else {
    #ifdef DEBUG
    cerr << red;
    cerr << "!! OVERFLOW IN SUB()" << endl;
    cerr << normal;
    #endif
    return false;
  }
}

// Adds the input to the specified register
// TODO: Verify overflow checking
bool ADR() {
  unsigned short int *reg = getRegister();
  unsigned short int addr = getAddrMode(machine.IR);
  unsigned short int operand = getOperand(machine.IR);
  int over = (int)*reg;

  *reg += (addr == DIRECT ? main_memory[MMU(operand)] : operand);
  over += (int)(addr == DIRECT ? main_memory[MMU(operand)] : operand);

  #ifdef DEBUG
  printDebug("ADR");
  #endif

  // Effect condition code; return false if overflow
  if (over == (int)*reg) {
    machine.CR = getCondCode(*reg);
    return true;
  } else {
    #ifdef DEBUG
    cerr << red;
    cerr << "!! OVERFLOW IN ADR()" << endl;
    cerr << normal;
    #endif
    return false;
  }
}

// Subtracts the input from the specified register
// TODO: Verify overflow checking
bool SUR() {
  unsigned short int *reg = getRegister();
  unsigned short int addr = getAddrMode(machine.IR);
  unsigned short int operand = getOperand(machine.IR);
  int over = (int)*reg;

  *reg -= (addr == DIRECT ? main_memory[MMU(operand)] : operand);
  over -= (int)(addr == DIRECT ? main_memory[MMU(operand)] : operand);

  #ifdef DEBUG
  printDebug("SUR");
  #endif

  // Effect condition code; return false if overflow
  if (over == (int)*reg) {
    machine.CR = getCondCode(*reg);
    return true;
  } else {
    #ifdef DEBUG
    cerr << red;
    cerr << "!! OVERFLOW IN SUR()" << endl;
    cerr << normal;
    #endif
    return false;
  }
}

// The And fuction will do the bitwise operation AND using the specified
// register, and the address of a value provided in the arguments
bool AND() {
  unsigned short int *reg = getRegister();
  unsigned short int operand =  main_memory[MMU(getOperand(machine.IR))];
  unsigned short int result = (*reg & operand);

  *reg = result; // set the new result into the register specified

  machine.CR = getCondCode(*reg);

  #ifdef DEBUG
  printDebug("AND");
  #endif
  return true;
}

// The IOR fuction will do the bitwise operation Or using the specified
// register, and the address of a value provided in the arguments
bool IOR() {
  unsigned short int *reg = getRegister();
  unsigned short int operand =  main_memory[MMU(getOperand(machine.IR))];
  unsigned short int result = (*reg | operand);

  *reg = result; // set the new result into the register specified

  machine.CR = getCondCode(*reg);

  #ifdef DEBUG
  printDebug("IOR");
  #endif
  return true;
}

// This is the bitwise NOT, or compliment function. This performs logical
// negation on each bit, forming the ones' complement of the given binary
// value. Bits that are 0 become 1, and those that are 1 become 0.
bool NOT() {

  unsigned short int *reg = getRegister();
  *reg = ~*reg;

  if (machine.CR != 0)
  machine.CR = getCondCode(*reg);

  #ifdef DEBUG
  printDebug("NOT");
  #endif
  return true;
}

// Unconditional Jump
/* Direct addressing is allowed, provided the value in the memory location
is actually only using 8 bits. If the 8 highest-order bits contain anything,
that would be a reference to a memory location that does not exist (PC: 8bits)
and thus the JMP would return an error (essentially OutOfBounds) */
bool JMP() {
  unsigned short int addr = getAddrMode(machine.IR);
  unsigned short int jmpTo;
  if (addr == DIRECT) {
    jmpTo = main_memory[MMU(getOperand(machine.IR))];
    // Check high-order bits
    if ((jmpTo & 65280) != 0)
    return false;
  } else if (addr == IMMEDIATE) {
    jmpTo = getOperand(machine.IR);
  } else {
    return false;
  }

  // Set program counter to new address
  machine.PC = jmpTo;

  #ifdef DEBUG
  if (getOpcode(machine.IR) == 9)
  printDebug("JMP");
  #endif
  return true;
}

// Check condition flag register; jump if appropriate
bool JEQ() {
  if (machine.CR == EQL)
  JMP();
  #ifdef DEBUG
  printDebug("JEQ");
  #endif
  return true;
}

// Check condition flag register; jump if appropriate
bool JGT() {
  if (machine.CR == GRT)
  JMP();
  #ifdef DEBUG
  printDebug("JGT");
  #endif
  return true;
}

// Check condition flag register; jump if appropriate
bool JLT() {
  if (machine.CR == LST)
  JMP();
  #ifdef DEBUG
  printDebug("JLT");
  #endif
  return true;
}

// This is the compare function, it compares a
// direct or indirect value to the specified
// register, it will always be register oriented
// I.E. It's always of the style of (reg < instruction)
bool CMP() {

  unsigned short int *reg = getRegister();
  unsigned short int operand = getOperand(machine.IR);
  unsigned short int addr = getAddrMode(machine.IR);
  unsigned short int left_operand, right_operand;

  left_operand = *reg;
  right_operand = (addr == DIRECT ? main_memory[MMU(operand)] : operand);

  if (left_operand < right_operand) {
    machine.CR = LST;
  } else if (left_operand == right_operand) {
    machine.CR = EQL;
  } else if (left_operand > right_operand) {
    machine.CR = GRT;
  }

  #ifdef DEBUG
  printDebug("CMP");
  #endif
  return true;
}

// Will clear the specified regiester with 0s
bool CLR() {
  unsigned short int *reg = getRegister();
  *reg = 0;
  machine.CR = 0;

  #ifdef DEBUG
  printDebug("CLR");
  #endif
  return true;
}

// Clears all registers; reset IR to HLT (61440)
bool HLT() {
  machine.rA = 0;
  machine.r1 = 0;
  machine.r2 = 0;
  machine.r3 = 0;
  machine.IR = 61440;
  machine.PC = 0;
  machine.CR = 0;

  #ifdef DEBUG
  printDebug("HLT");
  #endif
  return true;
}


// Determine if a resultant value is positive, negative, or 0
// Takes a short integer, returns a condition code (short integer)
unsigned short int getCondCode(unsigned short int x) {
  if (x == 0)
  return EQL;
  else if (x > 0)
  return GRT;
  else
  return LST;
}

// Take a register code and return a pointer to that register
unsigned short int* getRegister() {
  unsigned short int regCode = getRegCode(machine.IR); // Returns 0-3

  #ifdef DEBUG_VERBOSE
  cout << "In getRegister(). IR: " << machine.IR << \
    ", Regcode: %d" << regCode << endl;
  #endif

  switch (regCode) {
    case 1: return &machine.r1; break;
    case 2: return &machine.r2; break;
    case 3: return &machine.r3; break;
    default: return &machine.rA; break;
  }
}

#ifdef DEBUG
void printDebug(string op) {
  unsigned short int oldPC = machine.PC - 1;
  unsigned short int addr = getAddrMode(machine.IR);
  unsigned short int code = getRegCode(machine.IR);
  char reg = (code == 0 ? 'A' : code + '0');
  unsigned short int operand = getOperand(machine.IR);
  cout << "Instruction with opcode " << op << " finished executing." << endl;
  cout << "\t0x" << oldPC << ": " << op << " " << addr \
    << " r" << reg << " " << operand << endl;

  #ifdef DEBUG_VERBOSE
  unsigned short int rA = machine.rA;
  unsigned short int r1 = machine.r1;
  unsigned short int r2 = machine.r2;
  unsigned short int r3 = machine.r3;
  unsigned short int IR = machine.IR; // TODO: Convert this to hex or binary
  unsigned short int PC = machine.PC;
  unsigned short int CR = machine.CR;
  cout << "\tDumping Registers:" << endl;
  cout << "\trA: " << rA << ", r1: " << r1 << ", r2: " \
    << r2 << ", r3: " << r3 << endl;
  cout << "\tIR: " << IR << ", PC: " << PC << ", CR: " << CR << endl;
  #endif
}
#endif
