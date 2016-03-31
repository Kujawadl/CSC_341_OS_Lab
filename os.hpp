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
#include "machine.hpp"
  using namespace std;

#ifndef OS
#define OS

// Define the user struct
enum userID {sys, u1, u2};
userID nextUser(userID user) {
  switch(user) {
    case sys: return u1; break;
    case u1: return u2; break;
    case u2: return sys; break;
  }
  return user;
}
struct Process {
  int id;
  int status;
  registers regs;
  // list<int>* tlb;
};

extern queue<Process> readyQueue;
extern queue<Process> blockedQueue;
extern Process currentProcess;

// Declare semaphor as enum (locked/unlocked) and READY and BLOCKED constants
enum e_semaphor {locked, unlocked} semaphor;
#define READY 1
#define BLOCKED 0
#define RUNNING 2
#define NONE 3

// Variables used by the scheduler
extern int sysclock;
extern int switchTime;
extern userID currentUser;
extern Process processTable[64];

// Dump contents of main memory and all registers and semaphore status
void dump();

// Print the contents of a queue, with a header containing the queue name
void printQueue(string queueName, queue<Process> &q,int num);

// Print the contents of a queue
void printQueue(queue<Process> &q,int num);

// If main memory is unlocked and a process requests it, lock it.
// If it's already locked, block the process.
bool semwait();

// Signal that main memory has become available, and the
// current program is no longer running.
void semsignal();

// Responsible for swapping users in and out, and controlling
// which users are blocked vs which are ready.
void dispatcher(int action);

// Round-robin scheduler, 3 ticks per user
void scheduler();

// Read program into main memory
void readFile();

// Convert a string command (run, dmp, etc) into an int that can be used
// in a switch case statement.
int cmdToInt(string);

// Case-insensitive string comparison
bool my_strcasecmp(string str1, string str2);

// Initialize all values required by the OS
void init();

// Main function (starts the OS)
int main(int argc, char** argv);

#endif

//Needed for formatting when printing
//</string></queue></iostream></iomanip>
//</cstdlib></cstring></sstream></fstream>
//</string></cctype></user></user></user></user>
