CPP = g++
GPP = g++
FLAGS = -Wall -Werror -O3
DEP_FLAGS = -Wall -Werror
MAIN_TARGET_BASE = main
TEST_TARGET_BASE = test
LIBRARIES =

# compiler dance:
# - default to g++
# - but use g++-4.7/g++-4.8 if they're on the $PATH
CPP47 = $(shell which c++-4.7 2>/dev/null)
CPP48 = $(shell which c++-4.8 2>/dev/null)
ICC   = $(shell which icpc 2>/dev/null)
ifneq ($(wildcard $(CPP47)),)
	CPP = g++-4.7
	GPP = g++-4.7
endif

ifneq ($(wildcard $(CPP48)),)
	CPP = g++-4.8
	GPP = g++-4.8
endif

ifneq ($(wildcard $(ICC)),)
	CPP = icpc
	LIBRARIES += -openmp
	# shoot me
	FLAGS += -wd444 -wd1418 -wd383 -wd981 -wd1599 -wd193
endif

ifeq ($(wildcard $(ICC)),)
	LIBRARIES += -fopenmp
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
	$(GPP) $(DEP_FLAGS) -M -MF $@ -MT $(@:%.d=%.o) $<

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

debug: FLAGS += -g -DTEST=1
debug: all
	lldb -- ./$(TEST_TARGET)


# perf
perf: all
	@bin/perf

taccrun:
	@rm -rf hmmperf.*
	@qsub bin/job_script

coarse_vs_fine:
	@rm -rf coarsevsfineperf.*
	@qsub bin/coarse_vs_fine_job_script


# preprocessor
pre:
	@python $(PREPROCESS_FILES) data/penn-treebank-wsj $(OUT) $(NUM)


# report

report: $(REPORT_HTML)

$(REPORT_HTML): $(REPORT_SRC) etc/template.html etc/marked.js
	python etc/buildreport.py < $< > $@


# cleanup

.PHONY: clean taccrun
clean:
	-rm -rf $(OBJ_DIR) $(VAR_DIR) $(MAIN_TARGET)

-include $(DEPFILES)

