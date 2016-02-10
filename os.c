#include "os.h"

registers U1 = {0, 0, 0, 0, 61440, 0, 0};
registers U2 = {0, 0, 0, 0, 61440, 0, 0};
registers SYS = {0, 0, 0, 0, 61440, 0, 0};

void dump(bool dumpRegs)
{
	if (dumpRegs == true)
	{
		/* Dump registers */
	}

	/* Dump memory */
}

void scheduler()
{
	/* Scheduler code */
    while (true) {

        Node currentUser = schedulerQueue.pop(&schedulerQueue);
        machine = currentUser.userRegister;

        while (clock < switchTime) {
            // do stuff
            clock++;
            if (currentUser.userNum > 0) {
                printf("USER %d > ", currentUser.userNum);
            } else {
                printf("SYS > ");
            }


        }

        currentUser.userRegister = machine;
        schedulerQueue.push(&schedulerQueue, &currentUser);

    }

}

int main(int argc, char** argv)
{
	/* Initialize main_memory with given data */
	readFile(); // read instructions into main_memory
    init(); // do things with the things
    scheduler(); // call the scheduler
	/* Call scheduler */
}

void init(){

    // queue schedulerQueue = createQueue();

    Queue schedulerQueue = createQueue();
    Node sys = { .userNum = 0, .next = NULL, .userRegister = {0, 0, 0, 0, 61440, 0, 0} };
    Node usr1 = { .userNum = 1, .next = NULL, .userRegister = {0, 0, 0, 0, 61440, 0, 0} };
    Node usr2 = { .userNum = 2, .next = NULL, .userRegister = {0, 0, 0, 0, 61440, 0, 0} };


    schedulerQueue.push(&schedulerQueue, &usr1);
    schedulerQueue.push(&schedulerQueue, &usr2);
    schedulerQueue.push(&schedulerQueue, &sys);

    // intialize the clock to zero
    clock = 0;
    // intialize switch time to clock+3, this will be the next point at which
    //  users are changed.
    switchTime=clock+3;
}

void readFile(){
    /* read input file into int main_memory[] */
    FILE *ifp;
    ifp = fopen("part1.dat", "r"); /* input file */

    /* validates file exists */
    if (ifp == NULL){perror("proj1a.dat");exit(0);}

    char str[32]; /* current line from file, had to increase it size... */
    int i = 0; /* index of main_memory */
    while(fgets(str, 18, ifp)){ /* plus 2 for new lines. */
         printf("%s\t", str);  /* DEBUG */
        main_memory[i] = (short)(strtoul(str, NULL, 2));
         printf("%d\n", main_memory[i]);  /* DEBUG */
        i++;
    }
    fclose(ifp);
}
