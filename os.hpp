#include <string>
#include <queue>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fstream>
#include <string>
#include "machine.hpp"
  using namespace std;

//Needed for formatting when printing
//</stdio.h></stdlib.h></string.h>

#ifndef OS
#define OS

// Define the user struct
enum userID {sys, u1, u2};
struct User {
  userID id;
  registers regs;
};

extern queue<User> readyQueue;
extern queue<User> blockedQueue;
extern User currentUser;
extern bool semaphore;



// Variables used by the scheduler
extern int sysclock;
extern int switchTime;


// Dump contents of main memory
void dump(bool dumpRegs);
void printQueue(queue<User> &s,int num);

void dispatcher(int action);

// Round-robin scheduler
void scheduler();

// Main function (O/S Initialization)
int main(int argc, char** argv);

// Read program into memory
unsigned short int readFile(unsigned short int);

void init();

bool semsignal();
bool semwait();


int cmdToInt(string);

#endif
