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
int nextPID;

queue<Process*> RQ1;
queue<Process*> RQ2;
queue<Process*> SQ1;
queue<Process*> SQ2;

// Dump contents of main memory and all registers
void dump()
{
	cout << textbox("Dumping scheduler queues");

	cout << "Current process: "
			 << currentProcess->pname << " (PID " << currentProcess->pid << ")"
			 << endl << endl;
	cout << "RQ1: " << qtos(RQ1) << endl;
	cout << "RQ2: " << qtos(RQ2) << endl;
	cout << "SQ1: " << qtos(SQ1) << endl;
	cout << "SQ2: " << qtos(SQ2) << endl;

	cout << textbox("Dumping user process states in order of anticipated execution");
  for (int i = 0; i < int(RQ1.size()); i++) {
    cout << RQ1.front()->toString();
    Process *tmp = RQ1.front();
    RQ1.pop();
    RQ1.push(tmp);
  }
  for (int i = 0; i < int(RQ2.size()); i++) {
    cout << RQ2.front()->toString();
    Process *tmp = RQ2.front();
    RQ2.pop();
    RQ2.push(tmp);
  }
  for (int i = 0; i < int(SQ1.size()); i++) {
    cout << SQ1.front()->toString();
    Process *tmp = SQ1.front();
    SQ1.pop();
    SQ1.push(tmp);
  }
  for (int i = 0; i < int(SQ2.size()); i++) {
    cout << SQ2.front()->toString();
    Process *tmp = SQ2.front();
    SQ2.pop();
    SQ2.push(tmp);
  }
  if (RQ1.size() + RQ2.size() + SQ1.size() + SQ2.size() == 0) {
    cout << "No user processes running at this time..." << endl;
  }


	// Dump memory
	cout << textbox("Dumping memory" + padding(80) + "(only displaying allocated frames)");
	// Print table header
	cout << setfill('0');
	cout << "*" << padding(78, '-') << "*" << endl;
  cout << "|" << padding(32) << "Memory contents" << padding(31) << "|" << endl;
	cout << "*" << padding(78, '-') << "*" << endl;
	cout << "|   Frame  |"; for (int i = 0; i < 4; i++) cout << "|  Addr: Value  |";
	cout << endl << "*" << padding(78, '-') << "*" << endl;

	// Print table contents
	for (int i = 0; i < 64; i++) {
    bool print = (framesInUse[i]);
    if (print) {
      cout << "|    " << setw(2) << i << "    |";
      cout << hex;
      for (int j = 0; j < 4; j++) {
        cout << "|  0x" << setw(2) << (i*4)+j
             << ": 0x" << setw(4) << main_memory[(i*4)+j] << " |";
      }
      cout << dec << endl;
    } else if (i <= 0 || framesInUse[i-1]) {
      cout << "|" << padding(38) << "..." << padding(37) << "|" << endl;
    }
	}
	cout << "*" << padding(78, '-') << "*" << endl;
	cout << setfill(' ');
}

// Dump everything (to be called on STP)
void fulldump() {
  cout << textbox("Dumping scheduler queues");

	cout << "Current process: "
			 << currentProcess->pname << " (PID " << currentProcess->pid << ")"
			 << endl << endl;
	cout << "RQ1: " << qtos(RQ1) << endl;
	cout << "RQ2: " << qtos(RQ2) << endl;
	cout << "SQ1: " << qtos(SQ1) << endl;
	cout << "SQ2: " << qtos(SQ2) << endl;

	cout << textbox("Dumping user process states in order of anticipated execution");
  for (int i = 0; i < int(RQ1.size()); i++) {
    cout << RQ1.front()->toString();
    Process *tmp = RQ1.front();
    RQ1.pop();
    RQ1.push(tmp);
  }
  for (int i = 0; i < int(RQ2.size()); i++) {
    cout << RQ2.front()->toString();
    Process *tmp = RQ2.front();
    RQ2.pop();
    RQ2.push(tmp);
  }
  for (int i = 0; i < int(SQ1.size()); i++) {
    cout << SQ1.front()->toString();
    Process *tmp = SQ1.front();
    SQ1.pop();
    SQ1.push(tmp);
  }
  for (int i = 0; i < int(SQ2.size()); i++) {
    cout << SQ2.front()->toString();
    Process *tmp = SQ2.front();
    SQ2.pop();
    SQ2.push(tmp);
  }
  if (RQ1.size() + RQ2.size() + SQ1.size() + SQ2.size() == 0) {
    cout << "No user processes running at this time..." << endl;
  }


	// Dump memory
	cout << textbox("Dumping memory" + padding(80) + "(displaying all frames)");
	// Print table header
	cout << setfill('0');
	cout << "*" << padding(78, '-') << "*" << endl;
  cout << "|" << padding(32) << "Memory contents" << padding(31) << "|" << endl;
	cout << "*" << padding(78, '-') << "*" << endl;
	cout << "|   Frame  |"; for (int i = 0; i < 4; i++) cout << "|  Addr: Value  |";
	cout << endl << "*" << padding(78, '-') << "*" << endl;

	// Print table contents
	for (int i = 0; i < 64; i++) {
    cout << "|    " << setw(2) << i << "    |";
    cout << hex;
    for (int j = 0; j < 4; j++) {
      cout << "|  0x" << setw(2) << (i*4)+j
           << ": 0x" << setw(4) << main_memory[(i*4)+j] << " |";
    }
    cout << dec << endl;
	}
	cout << "*" << padding(78, '-') << "*" << endl;
	cout << setfill(' ');

  //TODO: print contents of disk
}

// Loads the two user programs from disk into main memory, paging them as
// appropriate. Currently takes no args, returns no vals, everything hardcoded.
Process* loader(User &currentUser, short int diskAddr)
{
	unsigned short int currentPage = 0,
										 currentWord = 0,
										 currentInstr = 0;

  Process *newProcess = new Process(nextPID++, &currentUser);

	machine.PTBR = newProcess->regs.PTBR;
	do {
		currentInstr = disk[diskAddr];
		main_memory[MMU((currentPage<<2) + currentWord)] = currentInstr;
		// If at the last word in the page, increment currentPage
		currentPage = (currentWord == 3 ? currentPage + 1 : currentPage);
		// If at the last word in the page, reset currentWord to 0
		currentWord = (currentWord == 3 ? 0 : currentWord + 1);
		diskAddr++;
	} while (currentInstr != 61440);

  return newProcess;
}

void scheduler() {
	while (true) {
		currentProcess = NULL;


		// Elevate priority of priority-2 proc's with 0 time so far
		for (int i = 0; i < int(RQ2.size()); i++) {
			if (RQ2.front()->user->time == 0) {
				RQ1.push(RQ2.front());
			} else {
				RQ2.push(RQ2.front());
			}
			RQ2.pop();
		}
		// Just a quick print of the queue for Verification
		#ifdef DEBUG_VERBOSE
		cout << blue;
		cout << textbox("Dumping scheduler queues");
		cout << "RQ1: " << qtos(RQ1) << endl;
		cout << "RQ2: " << qtos(RQ2) << endl;
		cout << "SQ1: " << qtos(SQ1) << endl;
		cout << "SQ2: " << qtos(SQ2) << endl;
		cout << normal;
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
			if (currentProcess->user->id == sys) {
				cout << textbox("Switching to UI");
				// User interface is a special case
				userinterface();
				SQ1.push(currentProcess);
			} else {
				cout << textbox("Switching to Process " + itos(currentProcess->pid));
				int starttime = sysclock;
				// Load state, run, save state
				machine = currentProcess->regs;
				bool success = interpreter();
				// Save state and new proc run time
				currentProcess->regs = machine;
				currentProcess->user->time += sysclock - starttime;
				// If process encountered an error or halted, set running to false
				if (!success || machine.IR == 61440) {
					currentProcess->running = false;

					if (!success) {
						cerr << red;
						cerr << "\nError condition signaled! Killing process...\n";
						cerr << normal;
					}
				}
				// Return process to the shadow queue
				// If normal user process, return to S2
				if (currentProcess->user->id !=sys  && success && currentProcess->running) {
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

	for (int i = 0; i < 3; i++) {

    // Prompt for input
    switch (i) {
      case 0: cout << "SYS > "; break;
      case 1: cout << "U1 > "; break;
      case 2: cout << "U2 > "; break;
    }
		cin >> cmd;
		sysclock++;

    // Process the input
		switch (cmdToInt(cmd)) {
      //TODO: run should now be able to accept cmdline args specifying a program
      case 0: // "run"
        if (i == 0) {
          cout << red << "Invalid system command!" << normal << endl;
        } else if (i == 1) {
          Process* newProcess = loader(U1, 0);
          RQ2.push(newProcess);
          cout << "Process with PID " << newProcess->pid << " added to RQ2\n";
        } else if (i == 2) {
          Process* newProcess = loader(U2, 100);
          RQ2.push(newProcess);
          cout << "Process with PID " << newProcess->pid << " added to RQ2\n";
        }
				break;
			case 1: // "dmp"
				if (i == 0) {
          dump();
        } else {
          // If not system, dmp command is invalid
          cout << red << "Invalid user command!" << normal << endl;
        }
				break;
			case 2: // "nop"
        // Do nothing
				break;
			case 3: // "stp"
        if (i == 0) {
  				// If user programs did not cleanly terminate, exit failure
  				fulldump();
  				if (RQ1.empty() && RQ2.empty() && SQ1.empty() && SQ2.empty()) {
  					cout << green <<  "Exited cleanly. Goodbye." << normal << endl;
  					exit(EXIT_SUCCESS);
  				} else {
  					cerr << red;
  					cerr << "Force terminated; processes were still running." << endl;
  					cerr << normal;
  					exit(EXIT_FAILURE);
  				}
        } else {
          // If not system, stp command is invalid
          cout << red << "Invalid user command!" << normal << endl;
        }
				break;
			case 4: // "time"
        if (i == 0) {
          cout << "System clock is at "<< sysclock << " ticks" << endl;
        } else if (i == 1) {
          cout << "U1 clock is at "<< U1.time << " ticks" << endl;
        } else if (i == 2) {
          cout << "U2 clock is at "<< U2.time << " ticks" << endl;
        }
				break;
			default:
				cout << red << "Invalid command: " << cmd << normal << endl;
				break;
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

	// Initialize user registers; PTBR must be handled separately
	U1 = User(u1);
	U2 = User(u2);
	SYS = User(sys);

	RQ1 = queue<Process*>();
	RQ2 = queue<Process*>();
	SQ1 = queue<Process*>();
	SQ2 = queue<Process*>();

  nextPID = 0;
  Process *UI = new Process(nextPID++, &SYS, "UI");

	RQ1.push(UI);

	// Initialize sysclock
	sysclock = 0;
}

// Main function (starts the OS)
int main()
{
	// Print OS startup header
	cout << green;
	cout << titlebox("CSC 341 OS Lab" + padding(80) + \
		"Aaron Baker, Andrew Ballard, and Dylan Jager-Kujawa");
	cout << normal;

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
		out += (q.front()->user->id == 0 ? "ui" : itos(q.front()->user->id));
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
