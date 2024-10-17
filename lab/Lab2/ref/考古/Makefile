all: Lab2

CXX = g++

LINKFLAGS = -c -Wall

Lab2: main.o block.o bsTree.o nets.o
	$(CXX) -std=c++11 main.o block.o bsTree.o nets.o -o Lab2

main.o: main.cpp
	$(CXX) $(LINKFLAGS) main.cpp

block.o: block.cpp block.h
	$(CXX) $(LINKFLAGS) block.cpp

bsTree.o: bsTree.cpp bsTree.h
	$(CXX) $(LINKFLAGS) bsTree.cpp

nets.o: nets.cpp nets.h
	$(CXX) $(LINKFLAGS) nets.cpp

clean:
	rm -rf *.o Lab2






