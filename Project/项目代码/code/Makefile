FLAGS = -Wall -std=c++11 -fPIC -g

libmesh.so : mesh.o
	g++ $(FLAGS) mesh.o -shared -o libmesh.so

mesh.o : mesh.cpp mesh.h
	g++ $(FLAGS) -c mesh.cpp

main : main.o
	g++ $(FLAGS) main.o -L. -lmesh -Wl,-rpath=. -o main

main.o : main.cpp libmesh.so mesh.h
	g++ $(FLAGS) -c main.cpp

.PHONY : clean example debug
clean:
	rm -f *.o *.so main mymesh.m

example : main
	./main

debug:
	gdb -tui main
