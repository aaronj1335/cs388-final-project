CPP = g++
FLAGS = -Wall -Werror
MAIN_TARGET_BASE = main
TEST_TARGET_BASE = test
LIBRARIES = -fopenmp

# compiler dance:
# - default to g++
# - but use g++-4.7/g++-4.8 if they're on the $PATH
CPP47 = $(shell which c++-4.7)
CPP48 = $(shell which c++-4.8)
ifneq ($(wildcard $(CPP47)),)
	CPP = g++-4.7
endif

ifneq ($(wildcard $(CPP48)),)
	CPP = g++-4.8
endif

ifeq ($(DEBUG), 1)
	FLAGS += -g
endif

SRC_DIR = src
OBJ_DIR = obj
SCRATCH ?= .
VAR_DIR = $(SCRATCH)/var
BIN_DIR = bin
UTIL_DIR = util
MAIN_TARGET = $(BIN_DIR)/$(MAIN_TARGET_BASE)
TEST_TARGET = $(BIN_DIR)/$(TEST_TARGET_BASE)

INPUTS = $(wildcard $(SRC_DIR)/*.cpp)
INPUTS_TMP = $(subst $(SRC_DIR),$(OBJ_DIR),$(INPUTS))
OBJECTS = $(INPUTS_TMP:%.cpp=%.o)
MAIN_OBJECTS = $(filter-out $(OBJ_DIR)/$(TEST_TARGET_BASE).o,$(OBJECTS))
TEST_OBJECTS = $(filter-out $(OBJ_DIR)/$(MAIN_TARGET_BASE).o,$(OBJECTS))
DEPFILES = $(OBJECTS:%.o=%.d)

PREPROCESS_FILES = $(wildcard $(UTIL_DIR)/*.py)

REPORT_HTML = report/report.html
REPORT_SRC = report/report.md


# main application

all: $(MAIN_TARGET) $(TEST_TARGET)

$(MAIN_TARGET): $(MAIN_OBJECTS) | $(BIN_DIR)
	$(CPP) $(FLAGS) -o $@ $(MAIN_OBJECTS) $(LIBRARIES)

$(TEST_TARGET): $(TEST_OBJECTS) | $(BIN_DIR)
	$(CPP) $(FLAGS) -o $@ $(TEST_OBJECTS) $(LIBRARIES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CPP) $(FLAGS) -c -o $@ $< $(LIBRARIES)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)


# dependencies

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.hpp | $(OBJ_DIR)
	$(CPP) $(FLAGS) $(LIBRARIES) -M -MF $@ -MT $(@:%.d=%.o) $<

deps: $(DEPFILES)


# running, testing, etc

test: FLAGS += -g -DTEST=1
test: $(TEST_TARGET)
	@$(TEST_TARGET)

run: $(MAIN_TARGET)
	@$(MAIN_TARGET) $(ARGS)

$(VAR_DIR):
	mkdir $@

$(VAR_DIR)/1d.txt: $(VAR_DIR)
	@python test/perfdata.py 1 > $@

$(VAR_DIR)/4d.txt: $(VAR_DIR)
	@python test/perfdata.py 4 > $@

debug: all
	lldb -- ./$(TEST_TARGET)

# preprocessor
pre:
	@python $(PREPROCESS_FILES) data/penn-treebank-wsj $(OUT) $(NUM)

# report

report: $(REPORT_HTML)

$(REPORT_HTML): $(REPORT_SRC) etc/template.html etc/marked.js
	python etc/buildreport.py < $< > $@


# cleanup

.PHONY: clean
clean:
	-rm -rf $(OBJ_DIR) $(VAR_DIR) $(MAIN_TARGET)

-include $(DEPFILES)

