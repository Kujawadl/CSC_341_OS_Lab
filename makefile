all: os.o machine.o
	gcc -g machine.o os.o -o os

os.o: os.h os.c machine.o
	gcc -g -c os.c

machine.o: machine.h machine.c
	gcc -g -c machine.c