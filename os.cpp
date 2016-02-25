#include "os.hpp"

registers U1 = {0, 0, 0, 0, 0, 0, 0};
registers U2 = {0, 0, 0, 0, 0, 0, 0};
registers SYS = {0, 0, 0, 0, 0, 0, 0};

/*
Note: U1 & U2 instruction registers contain 61440 (HLT)
while user is not running any program. The run function
will later assign the IR to the value of the starting
memory address of the program.

The scheduler can quickly determine if a user is running
something by testing the opcode of the IR for HLT (1111).
*/

users currentUser;
int sysclock;
int switchTime;

void dump(bool dumpRegs)
{
	if (dumpRegs == true)
	{
		/* Dump registers */
	}
	/* Dump memory */
	printf("\n\t>>>>>>DUMPING MEMORY<<<<<<\n");
	int i;
	for (i = 0; i < 256; i++) {
		if (i % 4 == 0)
			printf("\n");
		else
			printf(" | ");
		printf("#%3d: %8d", i, main_memory[i]);
	}
	printf("\n\n>\tEnd of dump\n\n");

}

void scheduler()
{
	char const *titleFiller = "####################################################";
	char const *titleText = "################ CSC 341 OS Lab ####################";
	printf("\n%s\n%s\n%s\n\n", titleFiller, titleText, titleFiller);
	/* Scheduler code */
	while (true) {
		switch (currentUser) {
			case u1: machine = U1; break;
			case u2: machine = U2; break;
			case sys: machine = SYS; break;
		}

		while (sysclock < switchTime) {
			bool running = !(getOpcode(machine.IR) == 15);
			sysclock++;
			if (currentUser != sys && !running) {
				printf("USER%d > ", (currentUser == u1 ? 1 : 2));
			} else if (currentUser != sys && running) {
				interpreter();
			} else {
				printf("SYS > ");
			}
			if (!running || currentUser == sys) {
				char input[4];
				fgets(input, sizeof(input), stdin);
				/* Clear input buffer */
				char garbage[100];
				fgets(garbage, sizeof(garbage), stdin);

				if (strcasecmp(input, "run") == 0) {
					if (currentUser != sys) {
						machine.IR = main_memory[machine.PC];
						// interpreter();
					} else {
						printf("Invalid command for system\n");
					}
				} else if (strcasecmp(input, "dmp") == 0) {
					if (currentUser == sys) {
						dump(false);
					} else {
						printf("Invalid command for users\n");
					}
				} else if (strcasecmp(input, "nop") == 0) {
					break;
				} else if (strcasecmp(input, "stp") == 0) {
					if (currentUser == sys) {
						exit(EXIT_SUCCESS);
					} else {
						printf("Invalid command for users\n");
					}
				} else {
					printf("Invalid command: %s\n", input);
				}
			}
		}

		switch (currentUser) {
			case u1: U1 = machine; break;
			case u2: U2 = machine; break;
			case sys: SYS = machine; break;
		}

		switchTime = sysclock + 3;
		currentUser = nextUser(currentUser);

		printf("\n%s\n", titleFiller);
		printf("############### Switching to user %d ################\n", currentUser);
		printf("%s\n\n", titleFiller);
	}

}

int main(int argc, char** argv)
{
	init();
	scheduler();
}

void init(){
	/* Initialize main_memory */
	int i;
	for (i = 0; i < 256; i++) {
		main_memory[i] = 0;
	}

	/* Read in the program for u1 and u2 */
	int u1_start = 0;
	int u2_start = 100;
	readFile(u2_start);
	U1.IR = 61440;
	U1.PC = u1_start;
	U2.IR = 61440;
	U2.PC = u2_start;
	currentUser = u1;

	/* Initialize sysclock and switchTime */
	sysclock = 0;
	switchTime = sysclock + 3;
}

/* Read program into memory */
/* Takes the address to start loading into */
/* Returns the size of the program in words */
unsigned short int readFile(unsigned short int start){
	FILE *ifp;
	ifp = fopen("part2.dat", "r");

	/* Ensure file exists */
	if (ifp == NULL) {
		fprintf(stderr, "proj1a.dat does not exist!\n");
		exit(0);
	}

	char str[18];
	int i = start;
	while(fgets(str, 18, ifp)){
		if (str[0] == '*') {
			// This is hardcoded for project 2. Sorry dawg
			i = 100;
			continue;
		}

		main_memory[i] = (short int)(strtol(str, NULL, 2));
		i++;
	}
	fclose(ifp);

	return (i - start);
}
