SOURCE_DIR = ../src
BIN_DIR = ../bin
OBJ_DIR = ../obj
CFLAGS = -std=c++11 -g -c
LDFLAGS = -std=c++11 -g
EXEC = $(BIN_DIR)/Lab1
OUT_DIR = ../output
DRAW_DIR = ../draw
TESTCASE_DIR = ../testcase
ANS_DIR = ../answer

# Get all .cpp files in the source directory
SRC_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)

# Extract the base names of the .cpp files and append .o to create object file names
OBJ_FILES = $(patsubst $(SOURCE_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Get all .txt files in the testcase directory
TESTCASE_FILES = $(wildcard $(TESTCASE_DIR)/*.txt)

# Get all .txt files in the answer directory
ANS_FILES = $(wildcard $(ANS_DIR)/*.txt)

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
	rm ../layout/*.txt
	rm ../drawing/*.png

# Define a pattern rule for run targets
run_%: $(EXEC)
	timeout 60 ./$(EXEC) $(TESTCASE_DIR)/$*.txt $(OUT_DIR)/$*_out.txt > cmd_out.txt

run_valgrind_%: $(EXEC)
	valgrind --leak-check=full --log-file=memory.log ./$(EXEC) $(TESTCASE_DIR)/$*.txt $(OUT_DIR)/$*_out.txt > cmd_out.txt

# Run all testcases
run_all: $(EXEC)
	@$(foreach file,$(basename $(notdir $(TESTCASE_FILES))), \
		echo "Running $(file)"; \
		timeout 60 ./$(EXEC) $(TESTCASE_DIR)/$(file).txt $(OUT_DIR)/$(file)_out.txt; \
		printf "Finished running $(file)\n\n";)

# Check a specific testcase by number
check_case%:
	diff -b $(OUT_DIR)/case$*_out.txt $(ANS_DIR)/output$*.txt;

# Check all testcases
check_all:
	@$(foreach file,$(basename $(notdir $(TESTCASE_FILES))), \
		echo "Checking $(file)"; \
		diff -b $(OUT_DIR)/$(file)_out.txt $(ANS_DIR)/output$(file).txt; \
		printf "Finished checking $(file)\n\n";)
