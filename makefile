all: os.o machine.o
	g++ -std=c++98 -ggdb .utils.o .machine.o .os.o -o os

os.o: os.hpp os.cpp machine.o utils.o
	g++ -std=c++98 -ggdb -c os.cpp -o .os.o

machine.o: machine.hpp machine.cpp utils.o
	g++ -std=c++98 -ggdb -c machine.cpp -o .machine.o

utils.o: utils.hpp utils.cpp
	g++ -std=c++98 -ggdb -c utils.cpp -o .utils.o
