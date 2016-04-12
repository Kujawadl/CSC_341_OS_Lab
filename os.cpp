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

Process* currentProcess;

queue<Process*> RQ1;
queue<Process*> RQ2;
queue<Process*> SQ1;
queue<Process*> SQ2;

// Dump contents of main memory and all registers
void dump()
{
	cout << textbox("Dumping scheduler queues");

	cout << "Current process: "
			 << (currentProcess->id == 0 ? "UI" : itos(currentProcess->id))
			 << endl << endl;
	cout << "RQ1: " << qtos(RQ1) << endl;
	cout << "RQ2: " << qtos(RQ2) << endl;
	cout << "SQ1: " << qtos(SQ1) << endl;
	cout << "SQ2: " << qtos(SQ2) << endl;

	// Dump Registers
	cout << textbox("Dumping Process States:");

	cout << "Dumping Registers for PID = 1:" << endl;
	cout << "\trA: " << U1.proc->regs.rA \
			 << ", r1: " << U1.proc->regs.r1 \
			 << ", r2: " << U1.proc->regs.r2 \
			 << ", r3: " << U1.proc->regs.r3 << endl;
	cout << "\tIR: " << U1.proc->regs.IR
			 << ", PC: " << U1.proc->regs.PC \
			 << ", CR: " << U1.proc->regs.CR << endl;
	cout << "\tRunning: " << boolalpha << U1.proc->running
			 << ", Time: " << U1.proc->time << " ticks" << endl << endl;

	cout << "Dumping Registers for PID = 2:" << endl;
	cout << "\trA: " << U2.proc->regs.rA \
			 << ", r1: " << U2.proc->regs.r1 \
			 << ", r2: " << U2.proc->regs.r2 \
			 << ", r3: " << U2.proc->regs.r3 << endl;
	cout << "\tIR: " << U2.proc->regs.IR \
			 << ", PC: " << U2.proc->regs.PC \
			 << ", CR: " << U2.proc->regs.CR << endl;
	cout << "\tRunning: " << boolalpha << U2.proc->running
			 << ", Time: " << U2.proc->time << " ticks" << endl;

	// Dump memory
	cout << textbox("Dumping memory:");
	// Print table header
	cout << setfill('0');
	cout << "*"; for (int i = 0; i < 78; i++) cout << "-"; cout << "*" << endl;
	cout << "| Frame Table ||| ";
		for (int i = 0; i < 22; i++) cout << " "; cout << "Memory contents";
		for (int i = 0; i < 23; i++) cout << " "; cout << " |" << endl;
	cout << "*"; for (int i = 0; i < 78; i++) cout << "-"; cout << "*" << endl;
	cout << "| Fr# | InUse |";
		for (int i = 0; i < 4; i++) cout << "|| Addr: Value  ";
		cout << "|" << endl;
	cout << "*"; for (int i = 0; i < 78; i++) cout << "-"; cout << "*" << endl;

	// Print table contents
	for (int i = 0; i < 64; i++) {
		cout << "|  " << setw(2) << i << " |   "
				 << (framesInUse[i] ? "Y" : " ") << "   ";
		cout << "|" << hex;
		for (int j = 0; j < 4; j++) {
			cout << "|| 0x" << setw(2) << (i*4)+j
					 << ": 0x" << setw(4) << main_memory[(i*4)+j]
					 << " ";
		}
		cout << "|" << dec << endl;
	}
	cout << "*"; for (int i = 0; i < 78; i++) cout << "-"; cout << "*" << endl;
	cout << setfill(' ');

	cout << endl << "User1 Page Table:" << endl;

	U1.proc->regs.PTBR->print();

	cout << endl << "User2 Page Table:" << endl;

	U2.proc->regs.PTBR->print();
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

void scheduler() {
	while (true) {
		currentProcess = NULL;


		// Elevate priority of priority-2 proc's with 0 time so far
		for (int i = 0; i < RQ2.size(); i++) {
			if (RQ2.front()->time == 0) {
				RQ1.push(RQ2.front());
			} else {
				RQ2.push(RQ2.front());
			}
			RQ2.pop();
		}
		// Just a quick print of the queue for Verification
		#ifdef DEBUG_VERBOSE
		  cout << textbox("Dumping scheduler queues");
		  cout << "RQ1: " << qtos(RQ1) << endl;
		  cout << "RQ2: " << qtos(RQ2) << endl;
		  cout << "SQ1: " << qtos(SQ1) << endl;
		  cout << "SQ2: " << qtos(SQ2) << endl;
		#endif
		// Search the queues for the next process
		if (!RQ1.empty()) {
			currentProcess = RQ1.front();
			RQ1.pop();
		} else if (!RQ2.empty()) {
			currentProcess = RQ2.front();
			RQ2.pop();
		// If no next process, set RQs to SQs
		} else {
			while (!SQ1.empty()) {
				RQ1.push(SQ1.front());
				SQ1.pop();
			}
			while (!SQ2.empty()) {
				RQ2.push(SQ2.front());
				SQ2.pop();
			}
		}

		if (currentProcess) {
			// Run the process
			if (currentProcess->id == sys) {
				cout << textbox("Switching to UI");
				// User interface is a special case
				userinterface();
				SQ1.push(currentProcess);
			} else {
				cout << textbox("Switching to Process " + itos(currentProcess->id));
				int starttime = sysclock;
				// Load state, run, save state
				machine = currentProcess->regs;
				bool success = interpreter();
				// Save state and new proc run time
				currentProcess->regs = machine;
				currentProcess->time += sysclock - starttime;
				// If process encountered an error or halted, set running to false
				if (!success || machine.IR == 61440) {
					currentProcess->running = false;
					currentProcess->time = 0;
				}
				// Return process to the shadow queue
				// If normal user process, return to S2
				if (currentProcess->id !=sys  && success && currentProcess->running) {
					SQ2.push(currentProcess);
				// If other process, return to S1
				} else if (success && currentProcess->running) {
					SQ1.push(currentProcess);
				}
			}
		}
	}
}

void userinterface() {
	string cmd;
	int timer_interrupt = 0;

	while (timer_interrupt < QUANTUM) {
		cout << "SYS > ";
		cin >> cmd;
		sysclock++;

		switch (cmdToInt(cmd)) {
			case 0: // "run"
				loader();
				cout << "The loader function was called" << endl;
				break;
			case 1: // "dmp"
				dump();
				break;
			case 2: // "nop"
				timer_interrupt = QUANTUM;
				break;
			case 3: // "stp"
				// If user programs did not cleanly terminate, exit failure
				dump();
				if (!U1.proc->running && !U2.proc->running) {
					cout << "Exited cleanly. Goodbye." << endl;
					exit(EXIT_SUCCESS);
				} else {
					cout << "Force terminated; processes were still running." << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case 4: // "time"
				cout << "System clock is at "<< sysclock << " ticks" << endl;
				break;
			default:
				cout << "Invalid command: " << cmd << endl;
				break;
		}
		timer_interrupt++;
	}
	timer_interrupt = 0;

	while(timer_interrupt < QUANTUM) {
		cout << "USER1 > ";
		cin >> cmd;
		sysclock++;

		switch(cmdToInt(cmd)) {
			case 0: // "run"
				if (U1.proc->running) {
					cout << "User 1 already has a running process!" << endl;
				} else {
					#ifdef DEBUG
					cout << "Adding P1 to RQ2" << endl;
					#endif
					RQ2.push(U1.proc);
					U1.proc->running = true;
				}
				break;
			case 1: // "dmp"
				cout << "Invalid user command!" << endl;
				break;
			case 2: // "nop"
				timer_interrupt = QUANTUM;
				break;
			case 3: // "stp"
				cout << "Invalid user command!" << endl;
				break;
			case 4: // "time"
				cout << "User1 Process has run for " << U1.proc->time << " ticks" << endl;
				break;
			default:
				cout << "Invalid command: " << cmd << endl;
				break;
		}
		timer_interrupt++;
	}
	timer_interrupt = 0;

	while(timer_interrupt < QUANTUM) {
		cout << "USER2 > ";
		cin >> cmd;
		sysclock++;

		switch(cmdToInt(cmd)) {
			case 0: // "run"
				if (U2.proc->running) {
					cout << "User 2 already has a running process!" << endl;
				} else {
					#ifdef DEBUG
					cout << "Adding P2 to RQ2" << endl;
					#endif
					RQ2.push(U2.proc);
					U2.proc->running = true;
				}
				break;
			case 1: // "dmp"
				cout << "Invalid user command!" << endl;
				break;
			case 2: // "nop"
				timer_interrupt = QUANTUM;
				break;
			case 3: // "stp"
				cout << "Invalid user command!" << endl;
				break;
			case 4: // "time"
				cout << "User2 Process has run for " << U2.proc->time << " ticks" << endl;
				break;
			default:
				cout << "Invalid command: " << cmd << endl;
				break;
		}
		timer_interrupt++;
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
	else if (my_strcasecmp(cmd, "time")) return 4;
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
	disk[i++] = 4202;
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

	RQ1 = queue<Process*>();
	RQ2 = queue<Process*>();
	SQ1 = queue<Process*>();
	SQ2 = queue<Process*>();

	RQ1.push(SYS.proc);

	// Initialize sysclock
	sysclock = 0;
}

// Main function (starts the OS)
int main(int argc, char** argv)
{
	// Print OS startup header
	cout << titlebox("CSC 341 OS Lab" + padding(80) + \
		"Aaron Baker, Andrew Ballard, and Dylan Jager-Kujawa");
	// Initialization
	init();
	// Start scheduler
	scheduler();
	return 0;
}

// Convert process queue to string
string qtos(queue<Process*> q) {
	queue<Process*> backup = queue<Process*>();
	int size = q.size(); // q.size decreases as queue is traversed; need a static size
	string out = "";
	for (int i = 0; i < size; i++) {
		out += (q.front()->id == 0 ? "ui" : itos(q.front()->id));
		// If not last element, add arrow
		if (i < size) {
			out += " -> ";
		}

		backup.push(q.front());
		q.pop();
	}
	out += "\n";

	while (!backup.empty()) {
		q.push(backup.front());
		backup.pop();
	}

	return out;
}
