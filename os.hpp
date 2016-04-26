/*
 * os.hpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341
 * Spring 2016
 */

#include <string>   // string
#include <queue>    // queue
#include <iostream> // cout, cerr
#include <iomanip>  // boolalpha, setw, setfill, hex, dec, endl
#include <sstream>  // sstream
#include "machine.hpp"
#include "utils.hpp"
  using namespace std;

#ifndef OS
#define OS

extern FrameTable framesInUse;
extern FrameTable framesLocked;
#define LOCKED true
#define UNLOCKED false

// Define the user struct
enum userID {sys, u1, u2};

struct User {
  userID id;
  int time;

  User() {}
  User(userID uid) : id(uid), time(0) {}
};
struct Process {
  int pid;
  string pname;
  User *user;
  bool running;
  registers regs;

  Process(int id, User *uid) : pid(id), pname(""), user(uid), running(false),
    regs(0, 0, 0, 0, 61440, 0, 0) {}
  Process(int id, User *uid, string name) : pid(id), pname(name), user(uid),
    running(false), regs(0, 0, 0, 0, 61440, 0, 0) {}
};

extern User U1, U2, SYS;

extern Process* currentProcess;

#define READY 1
#define BLOCKED 0

// Dump contents of main memory and all registers
void dump();

// Dump everything (to be called on STP)
void fulldump();

// Print the contents of a queue, with a header containing the queue name
void printQueue(string queueName, queue<User> &q,int num);

// Print the contents of a queue
void printQueue(queue<User> &q,int num);

// Loads the two user programs from disk into main memory, paging them as
// appropriate. Currently takes no args, returns no vals, everything hardcoded.
void loader();

// Round-robin scheduler, 3 ticks per user
void scheduler();

// The user interface, prompting for input
void userinterface();

// Convert a string command (run, dmp, etc) into an int that can be used
// in a switch case statement.
int cmdToInt(string);

// Initialize all values required by the OS
void init();

// Main function (starts the OS)
int main();

// Convert process queue to string
string qtos(queue<Process*>);

#endif
