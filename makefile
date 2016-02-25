all: os.o machine.o semaphore.o baseconvert.o
	gcc -g .machine.o .os.o .semaphore.o .baseconvert.o -o os

os.o: os.h os.c machine.o
	gcc -g -c os.c -o .os.o

machine.o: machine.h machine.c
	gcc -g -c machine.c -o .machine.o

semaphore.o: semaphore.h semaphore.c
	gcc -g -c semaphore.c -o .semaphore.o

baseconvert.o: baseconvert.h baseconvert.c
	gcc -g -c baseconvert.c -o .baseconvert.o
