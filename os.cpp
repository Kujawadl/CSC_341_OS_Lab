/*
* os.cpp
*
* Baker, Ballard, Jager-Kujawa
* CSC 341
* Spring 2016
*/

#include "os.hpp"

queue<Process> readyQueue = queue<Process>();
queue<Process> blockedQueue = queue<Process>();
Process currentProcess;
bool semaphore = unlocked;

int sysclock;
int switchTime;
Users currentUser;
Process processTable[64];

// Dump contents of main memory and all registers and semaphore status
void dump()
{
	// Dump Registers
	Process temp;
	cout << endl << ">>>>>>DUMPING REGISTERS<<<<<<" << endl;
	// Dump semaphore
	cout << endl << "Semaphore status: " << \
	(semaphore == locked ? "Locked" : "Unlocked") << endl;
	printQueue("readyQueue", readyQueue, readyQueue.size());
	printQueue("blockedQueue", blockedQueue, blockedQueue.size());

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
	cout << endl << endl << ">\tEnd of dump" << endl << endl;
}

// Print the contents of a queue, with a header containing the queue name
void printQueue(string queueName, queue<Process> &q, int num)
{
	cout << endl << "\t>>>>>>QUEUE: " << queueName << "<<<<<<" << endl;
	printQueue(q, num);
}

// Print the contents of a queue
void printQueue(queue<Process> &q, int num)
{
	if(!num)
	{
		cout << endl;
		return;
	}
	Process curr= q.front();
	q.pop();
	registers regs = curr.regs;
	cout << "\tUSER " << curr.id << ": Dumping Registers..." << endl;
	cout << "\t\trA: " << regs.rA << ", r1: " << regs.r1 << ", r2: " \
	<< regs.r2 << ", r3: " << regs.r3 << endl;
	cout << "\t\tIR: " << regs.IR << ", PC: " << regs.PC << ", CR: " \
	<< regs.CR << endl;
	q.push(curr);
	printQueue(q,--num);
}

// If main memory is unlocked and a process requests it, lock it.
// If it's already locked, block the process.
// Returns true if user has to wait, false if user doesn't have to wait.
bool semwait()
{
	// If the memory is unlocked, lock it, return false
	if (semaphore == unlocked) {
		semaphore = locked;
		return false;
		// If memory is blocked, call dispatcher with BLOCKED keyword, return true
	} else {
		dispatcher(BLOCKED);
		return true;
	}
}

// Signal that main memory has become available, and the
// current program is no longer running.
void semsignal()
{
	string titleFiller = "####################"\
	"################################";
	if (semaphore == locked) {
		semaphore = unlocked;
		cout << titleFiller << endl;
		cout << "USER " << currentProcess.id << " HAS SIGNALED FOR " \
		"UNLOCK OF MEMORY" << endl;
		cout << titleFiller << endl << endl;

		int i = 0;
		for (int i = 0; i < blockedQueue.size(); i++) {
			Process x = blockedQueue.front();
			blockedQueue.pop();
			cout << titleFiller << endl;
			cout << "USER " << x.id << " WAS PUT BACK INTO READY QUEUE" << endl;
			cout << titleFiller << endl << endl;
			readyQueue.push(x);
		}
	}
}

// Responsible for swapping users in and out, and controlling
// which users are blocked vs which are ready.
void dispatcher(int action)
{
	string titleFiller = "####################################################";

	// if (action == READY) {
	// 	// Return user to readyQueue
	// 	readyQueue.push(currentProcess);
	// } else {
	// 	blockedQueue.push(currentProcess);
	// 	cout << endl << titleFiller;
	// 	cout << endl << "USER "  << currentProcess.id << " WAS DENIED ACCESS TO "\
	// 	<< "MEMORY AND PUT INTO" << endl << "THE BLOCKED QUEUE UNTIL ANOTHER " \
	// 	<< "PROCESS FINISHES";
	// 	cout << endl << titleFiller << endl << endl;
	// }
	// Load next user
	// currentProcess = readyQueue.front();
	// readyQueue.pop();

	// Output information about user switch

	cout << endl << titleFiller << endl;
	cout << "############### Switching to user " << currentUser \
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
		dispatcher(READY);
		switchTime = sysclock + 5;

		while (sysclock < switchTime) {
			int status = currentProcess.status;
			// bool running = currentProcess.running;
			// bool instrLoaded = !(getOpcode(machine.IR) == 15);

			if (currentUser != sys) {
				cout << "USER" << (currentUser == u1 ? 1 : 2) << " > ";
			} else {
				cout << "SYS > ";
			}

			// Take input from user on the first iteration or while no process is
			//	defined.
			if (sysclock == switchTime-5 || status == NONE) {
				string input;
				getline(cin, input);
				switch (cmdToInt(input)) {
					case 0: // "run"
					if (currentUser != sys) {

						// ### NEED TO UPDATE FOR PAGES ###
						machine.IR = main_memory[machine.PC];
						// If memory is free, run as usual.
						// if (semwait() == false) {
							// cout << endl << titleFiller;
							// cout << endl << "USER " << currentProcess.id << " REQUESTED MEM " \
							// << "ACCESS AND ACCESS WAS GRANTED." << endl << "USER MAY " \
							// << "EXECUTE INSTRUCTIONS.";
							// cout << endl << titleFiller << endl << endl;
							// currentProcess.running = true;
							interpreter();
							// If last instruction executed was HLT
							if (machine.IR == 61440) {
								// Signal semaphore is unlocked
								// semsignal();
								// currentProcess.running = false;
								// Exit loop (current user is done)
								switchTime = sysclock;
							}
							// If memory is locked, dispatch to blockedQueue, load next user
							// Reset switchtime (i.e. reset the loop) so next user gets
							// 5 ticks.
						// } else {
						// 	switchTime = sysclock + 5;
						// }

					} else {
						// ### NEED TO CALL LOADER NOW ###

						// cout << "Invalid command for system" << endl;
					}
					break;
					case 1: // "dmp"
					if (currentUser == sys) {
						dump();
					} else {
						cout << "Invalid command for users" << endl;
					}
					break;
					case 2: // "nop"
					// Exit loop (current user is done)
					switchTime = sysclock-1;
					break;
					case 3: // "stp"
					if (currentUser == sys) {
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

			sysclock++;

			// // If current user is not running, prompt for a command
			// if (currentProcess.id != sys && status != RUNNING && !instrLoaded) {
			// 	cout << "USER" << (currentProcess.id == u1 ? 1 : 2) << " > ";
			// 	// If current user is not running, but has an instruction loaded, begin
			// 	// executing instructions (user just restored from blocked state)
			// } else if (currentProcess.id != sys && !running && instrLoaded) {
			// 	// If memory is free, run as usual.
			// 	if (semwait() == false) {
			// 		cout << endl << titleFiller;
			// 		cout << endl << "USER " << currentProcess.id << " REQUESTED MEM " \
			// 		<< "ACCESS AND ACCESS WAS GRANTED." << endl << "USER MAY " \
			// 		<< "EXECUTE INSTRUCTIONS.";
			// 		cout << endl << titleFiller << endl << endl;
			// 		currentProcess.running = true;
			// 		interpreter();
			// 		// If last instruction executed was HLT
			// 		if (machine.IR == 61440) {
			// 			// Signal semaphore is unlocked
			// 			semsignal();
			// 			currentProcess.running = false;
			// 			// Exit loop (current user is done)
			// 			switchTime = sysclock;
			// 		}
			// 		// If memory is locked, dispatch to blockedQueue, load next user
			// 		// Reset switchtime (i.e. reset the loop) so next user gets
			// 		// 3 ticks.
			// 	} else {
			// 		switchTime = sysclock + 3;
			// 		break;
			// 	}
			// 	// If current user is system, prompt as system
			// } else if (currentProcess.id != sys && running) {
			// 	interpreter();
			// 	// If last instruction executed was HLT
			// 	if (machine.IR == 61440) {
			// 		// Signal semaphore is unlocked
			// 		semsignal();
			// 		currentProcess.running = false;
			// 		// Exit loop (current user is done)
			// 		switchTime = sysclock;
			// 	}
			// } else {
			// 	cout << "SYS > ";
			// }
		}
		currentUser++;
	}
}

// Read program into memory
void readFile()
{
	// This method will need to be rewritten entirely using c++ streams
	ifstream infile("part3.dat");
	int i = 0;
	short unsigned int current;
	string line;
	while (std::getline(infile, line))
	{
		if (line == "*") {
			i = 100;
			continue;
		}
		char * ptr;
		current = strtol(line.c_str(), & ptr, 2);
		disk[i] = current;
		i++;
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

// Case-insensitive string comparison
bool my_strcasecmp(string str1, string str2)
{
	int length1 = str1.length();
	for (int i = 0; i < length1; i++) {
		char &c = str1.at(i);
		c = tolower(c);
	}
	int length2 = str2.length();
	for (int i = 0; i < length2; i++) {
		char &c = str2.at(i);
		c = tolower(c);
	}
	return ((str1 == str2) ? true : false);
}

// Initializes all values required by the OS
void init()
{
	// Initialize main_memory
	for (int i = 0; i < 255; i++) {
		main_memory[i] = 0;
	}
	for (int i = 0; i < 512; i++) {
		disk[i] = 0;
	}
	semaphore = unlocked;

	registers defaultRegisterValues =
	{				// Registers:
		0,			// R1
		0,			// R2
		0,			// R3
		0,			// RA
		61440,	// IR
		0,			// PC
		0				// CR
	};

	Process U1 = {u1, NONE, defaultRegisterValues};
	Process U2 = {u2, NONE, defaultRegisterValues};
	Process SYS = {sys, NONE, defaultRegisterValues};

	// processTable[0] = SYS;
	// processTable[1] = U1;
	// processTable[2] = U2;

	// Sys is first in the round robin system.
	currentProcess = SYS;
	currentUser.ready.push(U1);
	currentUser.ready.push(U2);
	// Read in the program for u1 and u2
	readFile();

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

//Needed for formatting when printing
//</user></user></user></user></user></user>
