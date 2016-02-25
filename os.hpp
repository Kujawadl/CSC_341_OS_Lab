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

//Needed for formatting when printing
//</string></queue></iostream></iomanip>
//</cstdlib></cstring></sstream></fstream>
//</string></cctype>

#ifndef OS
#define OS

// Define the user struct
enum userID {sys, u1, u2};
struct User {
  userID id;
  bool running;
  registers regs;
};

extern queue<User> readyQueue;
extern queue<User> blockedQueue;
extern User currentUser;

// Declare semaphor as enum (locked/unlocked) and READY and BLOCKED constants
enum e_semaphor {locked, unlocked} semaphor;
#define READY 1
#define BLOCKED 0



// Variables used by the scheduler
extern int sysclock;
extern int switchTime;


// Dump contents of main memory
void dump();
void printQueue(string queueName, queue<User> &q,int num);
void printQueue(queue<User> &q,int num);

void dispatcher(int action);

// Round-robin scheduler
void scheduler();

// Main function (O/S Initialization)
int main(int argc, char** argv);

// Read program into memory
unsigned short int readFile(unsigned short int);

void init();

void semsignal();
bool semwait();


int cmdToInt(string);

bool my_strcasecmp(string str1, string str2);

#endif
