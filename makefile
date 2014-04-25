CPP = g++
FLAGS = -Wall
MAIN_TARGET_BASE = main
TEST_TARGET_BASE = test

# compiler dance:
# - default to g++
# - but use g++-4.7/g++-4.8 if they're on the $PATH
CPP47 = $(shell which c++-4.7)
CPP48 = $(shell which c++-4.8)
ifneq ($(wildcard $(CPP47)),)
	CPP = g++-4.7
	LIBRARIES = -L$(HOME)/boost/lib -I$(HOME)/boost/include
endif

ifneq ($(wildcard $(CPP48)),)
	CPP = g++-4.8
	LIBRARIES = -L$(HOME)/boost/lib -I$(HOME)/boost/include
endif

ifeq ($(DEBUG), 1)
	FLAGS += -g
endif

# don't error out on warnings in CI builds
ifndef TRAVIS
	# we need -Wno-unused-local-typedefs because of something included for
	# boost ranges
	#
	# we need -Wno-return-local-addr because file_line_iterator::dereference()
	# returns a reference to a string. i haven't found a way around this.
	FLAGS += -Werror -Wno-unused-local-typedefs -Wno-return-local-addr
endif

LIBRARIES += -fopenmp -lboost_filesystem -lboost_system

SRC_DIR = src
OBJ_DIR = obj
SCRATCH ?= .
VAR_DIR = $(SCRATCH)/var
BIN_DIR = bin
MAIN_TARGET = $(BIN_DIR)/$(MAIN_TARGET_BASE)
TEST_TARGET = $(BIN_DIR)/$(TEST_TARGET_BASE)

INPUTS = $(wildcard $(SRC_DIR)/*.cpp)
INPUTS_TMP = $(subst $(SRC_DIR),$(OBJ_DIR),$(INPUTS))
OBJECTS = $(INPUTS_TMP:%.cpp=%.o)
MAIN_OBJECTS = $(filter-out $(OBJ_DIR)/$(TEST_TARGET_BASE).o,$(OBJECTS))
TEST_OBJECTS = $(filter-out $(OBJ_DIR)/$(MAIN_TARGET_BASE).o,$(OBJECTS))
DEPFILES = $(OBJECTS:%.o=%.d)

PERF_FILES = $(VAR_DIR)/1d.txt $(VAR_DIR)/4d.txt
PERF_SUBDIR = default
RESULTS_DIR = results/$(PERF_SUBDIR)

ifeq ($(OMP_NUM_THREADS), 1)
	PERF_SUBDIR = core
endif

ifeq ($(OMP_NUM_THREADS), 6)
	PERF_SUBDIR = socket
endif

ifeq ($(OMP_NUM_THREADS), 12)
	PERF_SUBDIR = node
endif

REPORT_HTML = report/report.html
REPORT_SRC = report/report.md

# main application

all: $(MAIN_TARGET) $(TEST_TARGET)

$(MAIN_TARGET): $(MAIN_OBJECTS) | $(BIN_DIR)
	$(CPP) $(FLAGS) $(LIBRARIES) -o $@ $(MAIN_OBJECTS)

$(TEST_TARGET): $(TEST_OBJECTS) | $(BIN_DIR)
	$(CPP) $(FLAGS) $(LIBRARIES) -o $@ $(TEST_OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CPP) $(FLAGS) $(LIBRARIES) -c -o $@ $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)


# dependencies

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.hpp | $(OBJ_DIR)
	$(CPP) $(FLAGS) $(LIBRARIES) -M -MF $@ -MT $(@:%.d=%.o) $<

deps: $(DEPFILES)


# running, testing, etc

test: $(TEST_TARGET)
	@$(TEST_TARGET)

$(VAR_DIR):
	mkdir $@

$(VAR_DIR)/1d.txt: $(VAR_DIR)
	@python test/perfdata.py 1 > $@

$(VAR_DIR)/4d.txt: $(VAR_DIR)
	@python test/perfdata.py 4 > $@

debug: all
	lldb -- ./$(TEST_TARGET)

# report

report: $(REPORT_HTML)

$(REPORT_HTML): $(REPORT_SRC) etc/template.html etc/marked.js
	python etc/buildreport.py < $< > $@


# cleanup

.PHONY: clean
clean:
	-rm -rf $(OBJ_DIR) $(VAR_DIR) $(MAIN_TARGET)

-include $(DEPFILES)

