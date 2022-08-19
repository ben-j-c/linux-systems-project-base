CC=gcc
INCLUDES= -I ./src/global
CFLAGS = -Werror -Wextra -Wall -MD
LFLAGS = -lutil -ldl -lc -lbsd
EXE_NAME = PROJECT_NAME
EXE_NAME := ./bin/$(EXE_NAME)
SRC := $(shell find src/ -type f -regex ".*\.c")
OBJ = $(patsubst %.c,%.o,$(patsubst src/%,obj/%,$(SRC))) # src/main.c -> obj/main.c -> obj/main.o

.PHONY: debug
debug: CFLAGS += -g -O0
debug: all

.PHONY: all
all: $(EXE_NAME)
	\

obj/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

$(EXE_NAME): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(OBJ) $(LFLAGS) -o $(EXE_NAME)

.PHONY: install
install: all
	\

### Utility
.PHONY: clean
clean:
	-rm $(OBJ)
	-rm -r obj/
	-rm $(EXE_NAME)

-include $(OBJ:.o=.d)