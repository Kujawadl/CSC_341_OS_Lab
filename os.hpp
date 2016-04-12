/*
 * os.hpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341
 * Spring 2016
 */

#include <string>
#include <queue>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>
#include <unistd.h>
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
// userID& operator++ (userID& curr) {
//   switch (curr) {
//     case sys: curr = u1; break;
//     case u1: curr = u2; break;
//     case u2: curr = sys; break;
//   }
//   return curr;
// }
// userID operator++ (userID& curr, int) {
//   userID orig = curr;
//   switch (curr) {
//     case sys: curr = u1; break;
//     case u1: curr = u2; break;
//     case u2: curr = sys; break;
//   }
//   return orig;
// }
struct Process {
  userID id;
  int time;
  bool running;
  registers regs;

  Process(userID uid) : id(uid), time(0), running(false),
    regs(0, 0, 0, 0, 61440, 0, 0) {}
};
struct User {
  userID id;
  Process* proc;

  User() : proc(NULL) {}
  User(userID uid) : id(uid), proc(new Process(uid)) {}
};

extern User U1, U2, SYS;
extern User currentUser;

#define READY 1
#define BLOCKED 0

// Dump contents of main memory and all registers
void dump();

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
int main(int argc, char** argv);

// Convert process queue to string
string qtos(queue<Process*>);

#endif
