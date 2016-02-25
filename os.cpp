#include "os.hpp"

/*
Note: U1 & U2 instruction registers contain 61440 (HLT)
while user is not running any program. The run function
will later assign the IR to the value of the starting
memory address of the program.

The scheduler can quickly determine if a user is running
something by testing the opcode of the IR for HLT (1111).
*/

queue<User> readyQueue = queue<User>();
queue<User> blockedQueue = queue<User>();
User currentUser;

int sysclock;
int switchTime;

void dump(bool dumpRegs)
{
	// Dump Registers
	User temp;
	cout << "\n\t>>>>>>DUMPING REGISTERS<<<<<<\n";
	printQueue(readyQueue, readyQueue.size());
	printQueue(blockedQueue, blockedQueue.size());

	// Dump memory
	cout << "\n\t>>>>>>DUMPING MEMORY<<<<<<\n";
	int i;
	for (i = 0; i < 256; i++) {
		if (i % 4 == 0)
		cout << endl;
		else
		cout << " | ";
		cout << "#" << setw(3) << i << ": " << setw(8) << main_memory[i];
	}
	cout << "\n\n>\tEnd of dump\n\n";

}

void printQueue(queue<User> &s,int num)
{
    if(!num)
    {
        cout << endl;
        return;
    }
    User x= s.front();
    s.pop();
    cout << "\nUSER " << x.id << " REGISTERS";
		cout << "\n\trA: " << x.regs.rA << ", r1: " << x.regs.r1 << ", r2: " \
			<< x.regs.r2 << ", r3: " << x.regs.r3 << endl;
		cout << "\tIR: " << x.regs.IR << ", PC: " << x.regs.PC << ", CR: " << x.regs.CR << endl;
    s.push(x);
    printQueue(s,--num);
}

void dispatcher() {
	// Save user state
	currentUser.regs = machine;
	// Return user to readyQueue
	readyQueue.push(currentUser);
	// Load next user
	currentUser = readyQueue.front();
	readyQueue.pop();
	// Load user state
	machine = currentUser.regs;

	// Output information about user switch
	string titleFiller = "####################################################";
	cout << endl << titleFiller << endl;
	cout << "############### Switching to user " << currentUser.id \
	<< " ################" << endl;
	cout << titleFiller << endl << endl;
}

void scheduler()
{
	// Scheduler code
	while (true) {
		// Load next user and assign its max time (3 ticks)
		// dispatcher();
		switchTime = sysclock + 3;

		while (sysclock < switchTime) {
			bool running = !(getOpcode(machine.IR) == 15);
			sysclock++;
			// If current user is not running, prompt for a command
			if (currentUser.id != sys && !running) {
				cout << "USER" << (currentUser.id == u1 ? 1 : 2) << " > ";
				// If current user is running, continue execution
			} else if (currentUser.id != sys && running) {
				interpreter();
				// If last executed command was halt, call semsignal and break
				// if (getOpCode(machine.IR) == 15) {
				//   semsignal();
				//	 break;
				// }
				// If current user is system, prompt as system
			} else {
				cout << "SYS > ";
			}

			// If necessary, take input from command line
			if (!running || currentUser.id == sys) {
				string input;
				getline(cin, input);

				switch (cmdToInt(input)) {
					case 0: // "run"
					if (currentUser.id != sys) {
						machine.IR = main_memory[machine.PC];
						// semwait();
						interpreter();
						// semsignal if necessary
					} else {
						cout << "Invalid command for system" << endl;
					}
					break;
					case 1: // "dmp"
					if (currentUser.id == sys) {
						dump(false);
					} else {
						cout << "Invalid command for users" << endl;
					}
					break;
					case 2: // "nop"
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

int main(int argc, char** argv)
{
	char const *titleFiller = "####################################################";
	char const *titleText = "################ CSC 341 OS Lab ####################";
	cout << endl << titleFiller << endl << titleText << endl << titleFiller << endl << endl;
	init();
	scheduler();
	return 0;
}

void init(){
	// Initialize main_memory
	int i;

	for (i = 0; i < 255; i++) {
		main_memory[i] = 0;
	}


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
	User U1 =
	{
		u1,			// UID
		defaultRegisterValues
	};

	User U2 =
	{
		u2,			// UID
		defaultRegisterValues
	};
	User SYS =
	{
		sys,			// UID
		defaultRegisterValues
	};

	// Read in the program for u1 and u2
	int u1_start = 0;
	int u2_start = 100;
	readFile(u2_start);
	U1.regs.PC = u1_start;
	U2.regs.PC = u2_start;

	readyQueue.push(U1);
	readyQueue.push(U2);
	readyQueue.push(SYS);

	// Initialize sysclock and switchTime
	sysclock = 0;
	switchTime = sysclock + 3;
}

bool semsignal() {

}

// Read program into memory
// Takes the address to start loading into
// Returns the size of the program in words
unsigned short int readFile(unsigned short int start){
	// This method will need to be rewritten entirely using c++ streams
	ifstream infile("part2.dat");
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
		main_memory[i] = current;
	}

}

// Convert a command as a string into an integer value to simplify
// switch statements using commands.
int cmdToInt(string cmd) {
	if (strcasecmp(cmd.c_str(), "run")) return 0;
	else if (strcasecmp(cmd.c_str(), "dmp")) return 1;
	else if (strcasecmp(cmd.c_str(), "nop")) return 2;
	else if (strcasecmp(cmd.c_str(), "stp")) return 3;
	else return -1;
}
