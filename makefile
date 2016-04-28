all: ./bin/os.o ./bin/machine.o ./bin/fsys.o
	g++ -std=c++98 -pedantic -ggdb -W -Weffc++ -Wunused ./bin/.utils.o ./bin/.machine.o ./bin/.fsys.o ./bin/.os.o -o os

bin/os.o: os.hpp os.cpp ./bin/machine.o ./bin/fsys.o ./bin/utils.o
	g++ -std=c++98 -pedantic -ggdb -c -W -Weffc++ -Wunused os.cpp -o ./bin/.os.o

bin/machine.o: machine.hpp machine.cpp ./bin/utils.o
	g++ -std=c++98 -pedantic -ggdb -c -W -Weffc++ -Wunused machine.cpp -o ./bin/.machine.o

bin/fsys.o: fsys.hpp fsys.cpp ./bin/utils.o
	g++ -std=c++98 -pedantic -ggdb -c -W -Weffc++ -Wunused fsys.cpp -o ./bin/.fsys.o

bin/utils.o: utils.hpp utils.cpp
	g++ -std=c++98 -pedantic -ggdb -c -W -Weffc++ -Wunused utils.cpp -o ./bin/.utils.o
