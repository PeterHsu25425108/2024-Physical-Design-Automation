SOURCE_DIR = ../src
BIN_DIR = ../bin
OBJ_DIR = ../obj
CFLAGS = -std=c++11  -c -g
LDFLAGS = -std=c++11 -g
EXEC = $(BIN_DIR)/Legalizer
OUT_DIR = ../output
DRAW_DIR = ../draw
DRAW_TEXT_DIR = ../draw/text
TESTCASE_DIR = ../testcase

# Define variables for flexibility
CASE_NAME ?= testcase_name  # Set a default case if not provided

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
	g++ $(CFLAGS)  -o $(OBJ_DIR)/$(1).o $(SOURCE_DIR)/$(1).cpp
endef

# Generate compile rules for each .cpp file
$(foreach src,$(basename $(notdir $(SRC_FILES))),$(eval $(call compile_rule,$(src))))

# Ensure the object directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
# Ensure the output directory exists
$(OUT_DIR):
	mkdir -p $(OUT_DIR)
# Ensure the draw directory exists
$(DRAW_DIR):
	mkdir -p $(DRAW_DIR)
# Ensure the draw text directory exists
$(DRAW_TEXT_DIR):
	mkdir -p $(DRAW_TEXT_DIR)
# Ensure the testcase directory exists
$(TESTCASE_DIR):
	mkdir -p $(TESTCASE_DIR)
# Ensure the bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

../output_wang:
	mkdir -p ../output_wang

clean:
	rm  $(EXEC) $(OBJ_DIR)/*.o
#	rm  $(OUT_DIR)/*

clean_plot:
	rm  $(DRAW_DIR)/png/*
	rm  $(DRAW_DIR)/svg/*
clean_text:
	rm  $(DRAW_TEXT_DIR)/*

# run ../draw/plot.py to plot all .txt files in the DRAW_TEXT_DIR
# plot_all:
# 	python3 $(DRAW_DIR)/plot_all.py $(DRAW_TEXT_DIR)

# plot_iter%: $(DRAW_TEXT_DIR)/iter%_plot.txt
# 	python3 $(DRAW_DIR)/plot.py $(DRAW_TEXT_DIR)/iter$(subst plot_iter,,$@)_plot.txt

# plot_init: $(DRAW_TEXT_DIR)/init_plot.txt
# 	python3 $(DRAW_DIR)/plot.py $(DRAW_TEXT_DIR)/init_plot.txt
# plot_final: $(DRAW_TEXT_DIR)/final_plot.txt
# 	python3 $(DRAW_DIR)/plot.py $(DRAW_TEXT_DIR)/final_plot.txt

# Pattern rule to run [case_name] testcase with alpha = [alpha] using valgrind
run_valgrind: $(EXEC)
	valgrind --log-file=memory.log ./$(EXEC) ../testcase/$(CASE_NAME)/$(CASE_NAME).lg ../testcase/$(CASE_NAME)/$(CASE_NAME).opt ../output/$(CASE_NAME)_post.lg > cmd.txt

run: $(EXEC)
	time ./$(EXEC) ../testcase/$(CASE_NAME)/$(CASE_NAME).lg ../testcase/$(CASE_NAME)/$(CASE_NAME).opt ../output/$(CASE_NAME)_post.lg > cmd.txt

run_cmd: $(EXEC)
	time ./$(EXEC) ../testcase/$(CASE_NAME)/$(CASE_NAME).lg ../testcase/$(CASE_NAME)/$(CASE_NAME).opt ../output/$(CASE_NAME)_post.lg

run_wang: ../wang_exe/Legalizer
	time ./../wang_exe/Legalizer ../testcase/$(CASE_NAME)/$(CASE_NAME).lg ../testcase/$(CASE_NAME)/$(CASE_NAME).opt ../output_wang/$(CASE_NAME)_post.lg > cmd.txt

check_wang: ../output/$(CASE_NAME)_post.lg
	./Evaluator ../testcase/$(CASE_NAME)/$(CASE_NAME).lg ../testcase/$(CASE_NAME)/$(CASE_NAME).opt ../output_wang/$(CASE_NAME)_post.lg

check: ../output/$(CASE_NAME)_post.lg
	./Evaluator ../testcase/$(CASE_NAME)/$(CASE_NAME).lg ../testcase/$(CASE_NAME)/$(CASE_NAME).opt ../output/$(CASE_NAME)_post.lg
