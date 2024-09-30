EXEC = Lab1
CFLAGS = -std=c++11 -c
LDFLAGS = -std=c++11

all: $(EXEC)

$(EXEC): main.o Block.o Solver.o
	g++ $(LDFLAGS)  -o $@ $^

main.o: main.cpp Block.h Solver.h DEBUG.h
	g++ $(CFLAGS) main.cpp

Block.o: Block.cpp Block.h DEBUG.h
	g++ $(CFLAGS) Block.cpp

Solver.o: Solver.cpp Solver.h Block.h DEBUG.h
	g++ $(CFLAGS) Solver.cpp

clean:
	rm -rf $(EXEC) *.o