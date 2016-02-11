#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine.h"

#ifndef OS
#define OS

extern registers U1;
extern registers U2;
extern registers SYS;

/* Define the users type and the nextUser method */
typedef enum users {sys, u1, u2} users;
users nextUser(users user) {
  switch(user) {
    case sys: return u1; break;
    case u1: return u2; break;
    case u2: return sys; break;
  }
  return user;
}

/* Variables used by the scheduler */
extern users currentUser;
extern int clock;
extern int switchTime;


/* Dump contents of main memory */
void dump(bool dumpRegs);

/* Round-robin scheduler */
void scheduler();

/* Main function (O/S Initialization) */
int main(int argc, char** argv);

/* Read program into memory */
unsigned short int readFile(unsigned short int);

void init();

#endif
