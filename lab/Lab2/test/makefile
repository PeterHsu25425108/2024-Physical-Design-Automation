SOURCE_DIR = ../src
BIN_DIR = ../bin
OBJ_DIR = ../obj
CFLAGS = -std=c++11 -g -c
LDFLAGS = -std=c++11 -g
EXEC = $(BIN_DIR)/Lab2
OUT_DIR = ../output
DRAW_DIR = ../draw
TESTCASE_DIR = ../testcase

# Get all .cpp files in the source directory
SRC_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)

# Extract the base names of the .cpp files and append .o to create object file names
OBJ_FILES = $(patsubst $(SOURCE_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Get all .txt files in the testcase directory
TESTCASE_FILES = $(wildcard $(TESTCASE_DIR)/*.txt)

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	g++ $(LDFLAGS) -g -o $@ $^

# Rule to compile .cpp files to .o files
define compile_rule
$(OBJ_DIR)/$(1).o: $(SOURCE_DIR)/$(1).cpp $(wildcard $(SOURCE_DIR)/*.h) | $(OBJ_DIR)
	g++ -c $(CFLAGS) -g -o $(OBJ_DIR)/$(1).o $(SOURCE_DIR)/$(1).cpp
endef

# Generate compile rules for each .cpp file
$(foreach src,$(basename $(notdir $(SRC_FILES))),$(eval $(call compile_rule,$(src))))

# Ensure the object directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(EXEC) $(OBJ_DIR)/*.o
	rm -rf $(OUT_DIR)/*
	rm -rf $(BIN_DIR)/*
