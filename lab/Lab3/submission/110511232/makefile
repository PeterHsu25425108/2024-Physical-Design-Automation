CFLAGS = -std=c++11  -c
LDFLAGS = -std=c++11
EXEC = Legalizer

all: $(EXEC) 

$(EXEC): Inst.cpp legalize.cpp main.cpp PlaceRow.cpp readWrite.cpp solverAPI.cpp
	g++ $(LDFLAGS)  -o $@ $^ 

clean:
	rm  $(EXEC)

