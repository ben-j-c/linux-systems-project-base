CC=gcc
INCLUDES= -I $(shell pwd)/src/global
CFLAGS = -Werror -Wextra -Wall -MD
LFLAGS = -lutil -ldl -lc -lbsd
EXE_NAME = PROJECT_NAME
EXE_NAME := ./bin/$(EXE_NAME)
SRC := $(shell find src/ -type f -regex ".*\.c")
OBJ = $(patsubst %.c,%.o,$(patsubst src/%,obj/%,$(SRC))) # src/main.c -> obj/main.c -> obj/main.o
TESTS := $(shell find tests/ -type f -regex ".*\.c")
TESTS_OUT := $(patsubst %.c,%.out,$(patsubst tests/%,obj_tests/%,$(TESTS)))
#Settings
RELEASE := 0
DEBUG := 1
ERROR_STACK_DISABLE := 0
ERROR_STACK_BUFFER_BACKED := 0

ifeq ($(RELEASE), 1)
	DEBUG := 0
endif

ifeq ($(DEBUG), 1)
	CFLAGS += -g -O0
endif

ifeq ($(ERROR_STACK_DISABLE), 1)
	CFLAGS += -DES_NO_DEBUG
endif

ifeq ($(ERROR_STACK_BUFFER_BACKED), 1)
	CFLAGS += -DES_BUFFER_BACKED
endif

.PHONY: all
all: tests executable
	

.PHONY: executable
executable: $(EXE_NAME)
	

obj/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

$(EXE_NAME): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(OBJ) $(LFLAGS) -o $(EXE_NAME)

### Tests
.PHONY: tests
tests: $(OBJ) run_tests
	\

.PHONY: run_tests
run_tests: $(TESTS_OUT) $(foreach test,$(TESTS_OUT),run_test/$(test))
	\

.PHONY: run_test/%
run_test/%: %
	@echo -------- Testing $< --------
	@$<
	@echo --------- Done $< ----------


obj_tests/%.out: tests/%.c $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(filter-out obj/main.o,$(OBJ)) $(LFLAGS) $(CFLAGS) $(INCLUDES) -o $@ $<

### Utility
.PHONY: install
install: all
	\

.PHONY: clean
clean:
	-rm $(OBJ)
	-rm -r obj/
	-rm $(EXE_NAME)
	-rm $(TESTS_OUT)
	-rm -r obj_tests/

-include $(OBJ:.o=.d)
-include $(TEST_OUT: .out=.d)
