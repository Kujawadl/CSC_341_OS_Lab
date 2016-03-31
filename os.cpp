/*
 * os.cpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341
 * Spring 2016
 */

#include "os.hpp"

FrameTable framesInUse;
FrameTable framesLocked;

User U1, U2, SYS;
User currentUser;

int sysclock;
int switchTime;

// Dump contents of main memory and all registers
void dump()
{
	// Dump Registers
	User temp;
	cout << endl << ">>>>>>DUMPING REGISTERS<<<<<<" << endl;

	// Dump memory
	cout << endl << ">>>>>>DUMPING MEMORY<<<<<<" << endl;
	// Print table header
	cout << "_____________________________________" \
		<< "__________________________" << endl;
	cout << "ad: valu | ad: valu | ad: valu | ad: " \
		<< "valu | ad: valu | ad: valu" << endl;
	cout << "_____________________________________" \
		<< "__________________________" << endl;
	// Print table contents
	for (int i = 0; i < 256; i++) {
		if (i % 6 == 0)
		cout << endl;
		else
		cout << " | ";
		cout << hex;
		cout << setw(2) << i << ": " << setw(4) << main_memory[i];
		cout << dec;
	}

	cout << endl << endl << "User1 Page Table:" << endl;

	U1.proc->regs.PTBR->print();

	cout << endl << endl << "User2 Page Table:";

	U2.proc->regs.PTBR->print();

	cout << endl << endl;

	cout << endl << endl << ">\tEnd of dump" << endl << endl;
}

// Loads the two user programs from disk into main memory, paging them as
// appropriate. Currently takes no args, returns no vals, everything hardcoded.
void loader()
{
	unsigned short int currentPage,
										 currentWord,
										 currentInstr,
										 currentDiskAddr;
	User *currentUser;

	// For User 1, read from disk starting at location 0
	currentPage = 0;
	currentWord = 0;
	currentInstr = 0;
	currentDiskAddr = 0;
	currentUser = &U1;
	machine.PTBR = currentUser->proc->regs.PTBR;
	do {
		currentInstr = disk[currentDiskAddr];
		main_memory[MMU((currentPage<<2) + currentWord)] = currentInstr;
		// If at the last word in the page, increment currentPage
		currentPage = (currentWord == 3 ? currentPage + 1 : currentPage);
		// If at the last word in the page, reset currentWord to 0
		currentWord = (currentWord == 3 ? 0 : currentWord + 1);
		currentDiskAddr++;
	} while (currentInstr != 61440);

	// For User 2, read from disk starting at location 100
	currentPage = 0;
	currentWord = 0;
	currentInstr = 0;
	currentDiskAddr = 100;
	currentUser = &U2;
	machine.PTBR = currentUser->proc->regs.PTBR;
	do {
		currentInstr = disk[currentDiskAddr];
		main_memory[MMU((currentPage<<2) + currentWord)] = currentInstr;
		// If at the last word in the page, increment currentPage
		currentPage = (currentWord == 3 ? currentPage + 1 : currentPage);
		// If at the last word in the page, reset currentWord to 0
		currentWord = (currentWord == 3 ? 0 : currentWord + 1);
		currentDiskAddr++;
	} while (currentInstr != 61440);
}

// Responsible for swapping users in and out, and controlling
// which users are blocked vs which are ready.
void dispatcher(int action)
{
	string titleFiller = "####################################################";
	// Save user state
	currentUser.proc->regs = machine;
	switch (currentUser.id++) {
		case sys: currentUser = U1; break;
    case u1: currentUser = U2; break;
    case u2: currentUser = SYS; break;
	}
		machine = currentUser.proc->regs;
		// Output information about user switch

		cout << endl << titleFiller << endl;
		cout << "############### Switching to user " << currentUser.id \
		<< " ################" << endl;
		cout << titleFiller << endl << endl;
}

// Round-robin scheduler, 3 ticks per user
void scheduler()
{
	string titleFiller = "######################################" \
	 "##############";
	// Scheduler code
	while (true) {
		// Load next user and assign its max time (3 ticks)
		// NOTE: If we return to this point in the loop, the dispatcher was not
		// 			 called during last iteration, i.e. current user was not BLOCKED,
		// 			 i.e. current user must be READY.
		if (sysclock != 0)
			dispatcher(READY);
		switchTime = sysclock + 5;

		while (sysclock < switchTime) {
			bool running = currentUser.proc->running;
			bool instrLoaded = !(getOpcode(machine.IR) == 15);
			sysclock++;
			// If current user is not running, prompt for a command
			if (currentUser.id != sys && !running && !instrLoaded) {
				cout << "USER" << (currentUser.id == u1 ? 1 : 2) << " > ";
			// If current user is not running, but has an instruction loaded, begin
			// executing instructions (user just restored from blocked state)
			} else if (currentUser.id != sys && !running && instrLoaded) {
					cout << endl << titleFiller;
					cout << endl << "USER " << currentUser.id << " REQUESTED MEM " \
						<< "ACCESS AND ACCESS WAS GRANTED." << endl << "USER MAY " \
						<< "EXECUTE INSTRUCTIONS.";
					cout << endl << titleFiller << endl << endl;
					currentUser.proc->running = true;
					interpreter();
					// If last instruction executed was HLT
					if (machine.IR == 61440) {
						currentUser.proc->running = false;
						// Exit loop (current user is done)
						switchTime = sysclock;
					}
			// If current user is system, prompt as system
			} else if (currentUser.id != sys && running) {
				interpreter();
				// If last instruction executed was HLT
				if (machine.IR == 61440) {
					currentUser.proc->running = false;
					// Exit loop (current user is done)
					switchTime = sysclock;
				}
			} else {
				cout << "SYS > ";
			}

		// If necessary, take input from command line
		if ((!running  && !instrLoaded) || currentUser.id == sys) {
			string input;
			getline(cin, input);
			switch (cmdToInt(input)) {
				case 0: // "run"
					if (currentUser.id != sys) {
						machine.IR = main_memory[machine.PC];
							cout << endl << titleFiller;
							cout << endl << "USER " << currentUser.id << " REQUESTED MEM " \
								<< "ACCESS AND ACCESS WAS GRANTED." << endl << "USER MAY " \
								<< "EXECUTE INSTRUCTIONS.";
							cout << endl << titleFiller << endl << endl;
							currentUser.proc->running = true;
							interpreter();
							// If last instruction executed was HLT
							if (machine.IR == 61440) {
								currentUser.proc->running = false;
								// Exit loop (current user is done)
								switchTime = sysclock;
							//}
						// If memory is locked, dispatch to blockedQueue, load next user
						// Reset switchtime (i.e. reset the loop) so next user gets
						// 5 ticks.
						} else {
							switchTime = sysclock + 5;
						}
					} else {
						loader();
						cout << "The loader function was called" << endl;
					}
					break;
				case 1: // "dmp"
					if (currentUser.id == sys) {
						dump();
					} else {
						cout << "Invalid command for users" << endl;
					}
					break;
				case 2: // "nop"
					// Exit loop (current user is done)
					switchTime = sysclock;
					break;
				case 3: // "stp"
					if (currentUser.id == sys) {
						exit(EXIT_SUCCESS);
					} else {
						cout << "Invalid command for users" << endl;
					}
					break;
				default:
					cout << "Invalid command: " << input << endl;
					break;
				}
			}
		}
	}
}

// Convert a command as a string into an integer value to simplify
// switch statements using commands.
int cmdToInt(string cmd)
{
	if (my_strcasecmp(cmd, "run")) return 0;
	else if (my_strcasecmp(cmd, "dmp")) return 1;
	else if (my_strcasecmp(cmd, "nop")) return 2;
	else if (my_strcasecmp(cmd, "stp")) return 3;
	else return -1;
}

// Initializes all values required by the OS
void init()
{
	// Initialize main_memory
	for (int i = 0; i < 255; i++) {
		main_memory[i] = 0;
	}
	for (int i = 0; i < NUM_FRAMES; i++) {
		framesInUse[i] = false;
	}

	// Initialize disk
	int i = 0;
	disk[i++] = 2058;
	disk[i++] = 4102;
	disk[i++] = 2309;
	disk[i++] = 16640;
	disk[i++] = 4103;
	disk[i++] = 61440;
	for (i = i; i < 100; i++) {disk[i] = 0;}
	disk[i++] = 2073;
	disk[i++] = 4102;
	disk[i++] = 2309;
	disk[i++] = 20736;
	disk[i++] = 4103;
	disk[i++] = 61440;
	for (i = i; i < 512; i++) {disk [i] = 0;}

	// Initialize default register values
	registers defaultRegisterValues = registers
	(				// Registers:
		0,			// R1
		0,			// R2
		0,			// R3
		0,			// RA
		61440,	// IR
		0,			// PC
		0				// CR
	);

	// Initialize user registers; PTBR must be handled separately
	U1 = User(u1);
	U1.proc->regs.PTBR = new PageTable(framesInUse);
	U2 = User(u2);
	U2.proc->regs.PTBR = new PageTable(framesInUse);
	SYS = User(sys);
	// SYS does not need a page table; it has no pages

	currentUser = SYS;

	// Initialize sysclock and switchTime
	sysclock = 0;
	switchTime = sysclock + 5;
}

// Main function (starts the OS)
int main(int argc, char** argv)
{
	// Print OS startup header
	string titleFiller = "####################################################";
	string titleText = "################ CSC 341 OS Lab ####################";
	cout << endl << titleFiller << endl \
		<< titleText << endl \
		<< titleFiller << endl << endl;
	// Initialization
	init();
	// Start scheduler
	scheduler();
	return 0;
}
