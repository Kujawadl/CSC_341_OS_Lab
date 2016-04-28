all: os.o machine.o
	g++ -std=c++98 -pedantic -ggdb -W -Weffc++ -Wunused .utils.o .machine.o .os.o -o os

os.o: os.hpp os.cpp machine.o utils.o
	g++ -std=c++98 -pedantic -ggdb -c -W -Weffc++ -Wunused os.cpp -o .os.o

machine.o: machine.hpp machine.cpp utils.o
	g++ -std=c++98 -pedantic -ggdb -c -W -Weffc++ -Wunused machine.cpp -o .machine.o

utils.o: utils.hpp utils.cpp
	g++ -std=c++98 -pedantic -ggdb -c -W -Weffc++ -Wunused utils.cpp -o .utils.o
