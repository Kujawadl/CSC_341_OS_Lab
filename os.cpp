/*
 * os.cpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341
 * Spring 2016
 */

#include "os.hpp"

string systemStats;

FSYS fileSystem;

FrameTable framesInUse;
//FrameTable framesLocked;

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

  printAllProcs();

  // Dump memory
  cout << textbox("Compact Memory Dump" + padding(80) + \
                  "(only displaying in-use frames)");
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
  // call the print disk method only in use.
  fileSystem.printDisk(1);
}

// Dump everything (to be called only on STP)
void fulldump() {
  cout << textbox("Dumping scheduler queues");

  cout << "Current process: "
       << currentProcess->pname << " (PID " << currentProcess->pid << ")"
       << endl << endl;
  cout << "RQ1: " << qtos(RQ1) << endl;
  cout << "RQ2: " << qtos(RQ2) << endl;
  cout << "SQ1: " << qtos(SQ1) << endl;
  cout << "SQ2: " << qtos(SQ2) << endl;

  printAllProcs();

  // Dump memory
  cout << textbox("Complete Memory Dump" + padding(80) + "(displaying all frames)");
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

  // call the print disk method; all blocks.
  fileSystem.printDisk(0);
}

// Print all process control blocks
void printAllProcs() {
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
}

// Loads the file into a new PCB and returns a pointer to the new Process.
// Takes the user by reference, as well as the cmdline entry the user entered.
// cmdline entry is split into cmd and args on the first encountered whitespace.
Process* loader(User &currentUser, string pname)
{
  unsigned short int currentPage = 0,
                     currentWord = 0;

  Process *newProcess = new Process(pname, nextPID++, &currentUser);
  newProcess->regs.PTBR = new PageTable(framesInUse);

  machine.PTBR = newProcess->regs.PTBR;

  string filename = pname.substr(pname.find_first_of(" ") + 1, pname.size() - pname.find_first_of(" "));
  FileBuffer buffer = fileSystem[filename];
  if (buffer.size() < 1) throw out_of_range("No such filename: " + filename);

  for (int i = 0; (unsigned long)i < buffer.size(); i++) {
    main_memory[MMU((currentPage<<2) + currentWord)] = buffer[i];
    if (currentWord == 3) {
      currentPage++;
      currentWord = 0;
    } else {
      currentWord++;
    }
  }

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
    // If no next process, move SQs' contents to RQs
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
      int uid = currentProcess->user->id;
      string pname = currentProcess->pname;
      cout << textbox("Switching to Process \"" + pname
           + "\", owned by " + (uid == 0 ? "SYS" : "U" + itos(uid))
           + " with PID: " + itos(currentProcess->pid));
      // Run the process
      if (uid == sys) {
        switch (cmdToInt(pname)) {
          case 1: //dmp
            dump();
            break;
          case 4: //time
            cout << "System clock is at "<< *sysclock << " ticks" << endl;
            break;
          case 5: //stats
            if (currentProcess->running) {
              cout << systemStats;
              systemStats = "";
              currentProcess->running = false;
            } else {
              cout << "Gather statistics..." << endl;

              currentProcess->running = true;
              systemStats = textbox("System Statistics");

              bool u1_active = false, u2_active = false;
              for (int i = 0; i < int(RQ1.size()); i++) {
                if (RQ1.front()->user->id == 1) {
                  u1_active = true;
                } else if (RQ1.front()->user->id == 2) {
                  u2_active = true;
                }
                Process *tmp = RQ1.front();
                RQ1.pop();
                RQ1.push(tmp);
              }
              for (int i = 0; i < int(RQ2.size()); i++) {
                if (RQ2.front()->user->id == 1) {
                  u1_active = true;
                } else if (RQ2.front()->user->id == 2) {
                  u2_active = true;
                }
                Process *tmp = RQ2.front();
                RQ2.pop();
                RQ2.push(tmp);
              }
              for (int i = 0; i < int(SQ1.size()); i++) {
                if (SQ1.front()->user->id == 1) {
                  u1_active = true;
                } else if (SQ1.front()->user->id == 2) {
                  u2_active = true;
                }
                Process *tmp = SQ1.front();
                SQ1.pop();
                SQ1.push(tmp);
              }
              for (int i = 0; i < int(SQ2.size()); i++) {
                if (SQ2.front()->user->id == 1) {
                  u1_active = true;
                } else if (SQ2.front()->user->id == 2) {
                  u2_active = true;
                }
                Process *tmp = SQ2.front();
                SQ2.pop();
                SQ2.push(tmp);
              }

              systemStats += "Active Users: " + itos(1 + (u1_active ? 1 : 0) + \
                              (u2_active ? 1 : 0)) + "\n";

              systemStats += "Active disk blocks: " + itos(fileSystem.active()) +"\n";

              int count = 0;
              for (int i = 0; i < NUM_FRAMES; i++) {
                if (framesInUse[i]) count++;
              }
              systemStats += "Active Memory Frames: " + itos(count) + "\n";
              SQ1.push(currentProcess);
            }
            break;
          default:
            if (my_strcasecmp(pname.substr(0, pname.find_first_of(" \n")), "ui")) {
              userinterface();
              SQ1.push(currentProcess);
            }
            break;
        }
      } else {
        bool success;
        int starttime;
        switch (cmdToInt(pname)) {
          case 0: //run
            starttime = *sysclock;

            // If not running, load from disk
            if (currentProcess->running == false) {
              currentProcess = loader((uid == 1 ? U1 : U2), pname);
              currentProcess->running = true;
            }

            // Load state, run, save state
            machine = currentProcess->regs;
            success = interpreter();
            // Save state and new proc run time
            currentProcess->regs = machine;
            currentProcess->user->time += *sysclock - starttime;
            // If process encountered an error or halted, set running to false
            if (!success || machine.IR == 61440) {
              currentProcess->running = false;

              // if (!success) {
              //   cerr << red;
              //   cerr << "\nError condition signaled! Killing process...\n";
              //   cerr << normal;
              // }
            }
            // Return process to the shadow queue
            // If normal user process, return to S2
            if (success && currentProcess->running) {
              SQ2.push(currentProcess);
            // If other process, return to S1
            } else if (success && currentProcess->running) {
              SQ1.push(currentProcess);
            }
            break;
          case 4: //time
            cout << "U" << uid << " clock is at "
                 << currentProcess->user->time << " ticks" << endl;
            break;
        }
      }
    }
  }
}

void userinterface() {
  string cmd;

  for (int u = 0; u < 3; u++) {

    // Prompt for input
    switch (u) {
      case 0: cout << "SYS > "; break;
      case 1: cout << "U1 > "; break;
      case 2: cout << "U2 > "; break;
    }
    getline(cin, cmd);
    (*sysclock)++;

    Process* newProcess;

    // Process the input
    switch (cmdToInt(cmd)) {
      case 0: // "run"
        if (u == 0) {
          cout << red << "Invalid system command!" << normal << endl;
        } else {
          string filename;
          FileBuffer buffer;
          try {
            filename = cmd.substr(cmd.find_first_of(" ") + 1,
                                  cmd.size() - cmd.find_first_of(" "));
            cout << filename << endl;
            buffer = fileSystem[filename];
            if (buffer.size() < 1) throw out_of_range("");
            newProcess = loader((u == 1 ? U1 : U2), cmd);
            RQ2.push(newProcess);
            cout << "Process with PID " << newProcess->pid << " added to RQ2\n";
          } catch (out_of_range e) {
            cout << red << textbox("No such filename!") << normal;
            u--;
          }
        }
        break;
      case 1: // "dmp"
        if (u == 0) {
          newProcess = new Process(cmd, nextPID++, &SYS);
          RQ1.push(newProcess);
          cout << "Process with PID " << newProcess->pid << " added to RQ1\n";
        } else {
          // If not system, dmp command is invalid
          cout << red << "Invalid user command!" << normal << endl;
        }
        break;
      case 2: // "nop"
        // Do nothing
        break;
      case 3: // "stp"
        if (u == 0) {
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
        newProcess = new Process(cmd, nextPID++,
          (u == 0 ? &SYS :
          (u == 1 ? &U1 : &U2)));

        if (u == 0) {
          RQ1.push(newProcess);
          cout << "Time process (PID: " << newProcess->pid << ") added to RQ1\n";
        } else {
          RQ2.push(newProcess);
          cout << "Time process (PID: " << newProcess->pid << ") added to RQ2\n";
        }
        break;
      case 5: // "stats"
        if (u == 0) {
          newProcess = new Process(cmd, nextPID++, &SYS);
          RQ1.push(newProcess);
        } else {
          cout << red << "Invalid user command!" << normal << endl;
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
  cmd = cmd.substr(0, cmd.find_first_of(" \n"));
  if (my_strcasecmp(cmd, "run")) return 0;
  else if (my_strcasecmp(cmd, "dmp")) return 1;
  else if (my_strcasecmp(cmd, "nop")) return 2;
  else if (my_strcasecmp(cmd, "stp")) return 3;
  else if (my_strcasecmp(cmd, "time")) return 4;
  else if (my_strcasecmp(cmd, "stats")) return 5;
  else return -1;
}

// Initializes all values required by the OS
void init()
{
  systemStats = "";

  fileSystem = FSYS();
  fileSystem.load();

  // Initialize main_memory
  for (int i = 0; i < 255; i++) {
    main_memory[i] = 0;
  }
  for (int i = 0; i < NUM_FRAMES; i++) {
    framesInUse[i] = false;
  }

  // Initialize user registers; PTBR must be handled separately
  U1 = User(u1);
  U2 = User(u2);
  SYS = User(sys);

  RQ1 = queue<Process*>();
  RQ2 = queue<Process*>();
  SQ1 = queue<Process*>();
  SQ2 = queue<Process*>();

  nextPID = 0;
  Process *UI = new Process("UI", nextPID++, &SYS);

  RQ1.push(UI);

  // Initialize sysclock
  sysclock = new int(0);
}

// Main function (starts the OS)
int main()
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
    int uid = q.front()->user->id;
    int pid = q.front()->pid;
    out += (uid == 0 ? "SYS" : "U" + itos(uid)) + ": " + q.front()->pname + " (PID " + itos(pid) + ")";
    // If not last element, add arrow
    if (i < size - 1) {
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
