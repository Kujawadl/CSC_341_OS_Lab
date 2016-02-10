#include "os.h"

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
}

int main(int argc, char** argv)
{
	/* Initialize main_memory with given data */
	readFile();

	/* Call scheduler */
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
        /* printf("%s\t", str); */ /* DEBUG */
        main_memory[i] = (short)(strtoul(str, NULL, 2));
        /* printf("%d\n", main_memory[i]); */ /* DEBUG */
        i++;
    }
    fclose(ifp);
}
