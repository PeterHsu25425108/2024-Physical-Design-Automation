SOURCE_DIR = ../src
BIN_DIR = ../bin
OBJ_DIR = ../obj
CFLAGS = -std=c++11  -c
LDFLAGS = -std=c++11 
EXEC = $(BIN_DIR)/Lab2
OUT_DIR = ../output
DRAW_DIR = ../draw
DRAW_TEXT_DIR = ../draw/text
TESTCASE_DIR = ../testcase

# Define variables for flexibility
CASE_NAME ?= testcase_name  # Set a default case if not provided
ALPHA ?= 1.0                # Set a default alpha value if not provided

# Get all .cpp files in the source directory
SRC_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)

# Extract the base names of the .cpp files and append .o to create object file names
OBJ_FILES = $(patsubst $(SOURCE_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Get all .txt files in the testcase directory
TESTCASE_FILES = $(wildcard $(TESTCASE_DIR)/*.txt)

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	g++ $(LDFLAGS)  -o $@ $^

# Rule to compile .cpp files to .o files
define compile_rule
$(OBJ_DIR)/$(1).o: $(SOURCE_DIR)/$(1).cpp $(wildcard $(SOURCE_DIR)/*.h) | $(OBJ_DIR)
	g++ -c $(CFLAGS)  -o $(OBJ_DIR)/$(1).o $(SOURCE_DIR)/$(1).cpp
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
	rm -rf $(DRAW_TEXT_DIR)/*

clean_plot:
	rm -rf $(DRAW_DIR)/png/*
	rm -rf $(DRAW_DIR)/svg/*
clean_text:
	rm -rf $(DRAW_TEXT_DIR)/*

# run ../draw/plot.py to plot all .txt files in the DRAW_TEXT_DIR
plot_all:
	python3 $(DRAW_DIR)/plot_all.py $(DRAW_TEXT_DIR)

plot_iter%: $(DRAW_TEXT_DIR)/iter%_plot.txt
	python3 $(DRAW_DIR)/plot.py $(DRAW_TEXT_DIR)/iter$(subst plot_iter,,$@)_plot.txt

plot_init: $(DRAW_TEXT_DIR)/init_plot.txt
	python3 $(DRAW_DIR)/plot.py $(DRAW_TEXT_DIR)/init_plot.txt
plot_final: $(DRAW_TEXT_DIR)/final_plot.txt
	python3 $(DRAW_DIR)/plot.py $(DRAW_TEXT_DIR)/final_plot.txt

# Pattern rule to run [case_name] testcase with alpha = [alpha] using valgrind
run_valgrind: $(EXEC)
	valgrind --leak-check=full --track-origins=yes --log-file=memory.log ./../bin/Lab2 $(ALPHA) ../testcase/$(CASE_NAME)/$(CASE_NAME).block ../testcase/$(CASE_NAME)/$(CASE_NAME).nets ../output/$(CASE_NAME)/output.txt > cmd.txt
	./verifier $(ALPHA) ../testcase/$(CASE_NAME)/$(CASE_NAME).block ../testcase/$(CASE_NAME)/$(CASE_NAME).nets ../output/$(CASE_NAME)/output.txt

run: $(EXEC)
	time ./../bin/Lab2 $(ALPHA) ../testcase/$(CASE_NAME)/$(CASE_NAME).block ../testcase/$(CASE_NAME)/$(CASE_NAME).nets ../output/$(CASE_NAME)/output.txt > cmd.txt
	./verifier $(ALPHA) ../testcase/$(CASE_NAME)/$(CASE_NAME).block ../testcase/$(CASE_NAME)/$(CASE_NAME).nets ../output/$(CASE_NAME)/output.txt

check: $(EXEC)
	./verifier $(ALPHA) ../testcase/$(CASE_NAME)/$(CASE_NAME).block ../testcase/$(CASE_NAME)/$(CASE_NAME).nets ../output/$(CASE_NAME)/output.txt