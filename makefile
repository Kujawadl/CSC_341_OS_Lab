all: os.o machine.o
	g++ -std=c++98 -ggdb .machine.o .os.o -o os

os.o: os.hpp os.cpp machine.o
	g++ -std=c++98 -ggdb -c os.cpp -o .os.o

machine.o: machine.hpp machine.cpp
	g++ -std=c++98 -ggdb -c machine.cpp -o .machine.o
