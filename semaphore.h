
/* This will be the simplest kind of semaphore,
 * called a binary semaphore because it has two
 * states locked or unlocked. This works much like
 * traffic lights for our scheduler */

 #define UNLOCKED 1
 #define LOCKED 2

 /* Define boolean type */
 typedef enum { false, true } bool;

/* Declare same size memory to hold current status of memory */
 unsigned short int memory_access[256];

 /* This will attempt to lock a location specified by the scheduler */
 /* Returns a true or false, indicating success or failure of locking or failure to do so
  * due to it be in use already. */
bool lockLocation(unsigned short int location);

/* After usage of that memory locaiton, it can be set to free again. */
/* Returns a true or false, indicating success of unlocking, or something bad happened. */
bool unlockLocation(unsigned short int location);
