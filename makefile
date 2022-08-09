CC=gcc
LN=ln
INCLUDES= -I ./src/global
CFLAGS = -Werror -Wextra -Wall -MD -g -O0 -ansi
LFLAGS = -lutil -ldl -lc -lbsd
EXE_NAME = ./bin/PROJECT_NAME
SRC := $(shell find src/ -type f -regex ".*\.c")
OBJ = $(patsubst %.c,%.o,$(patsubst src/%,obj/%,$(SRC))) # src/main.c -> obj/main.c -> obj/main.o

.PHONY: debug ansi c99
debug: CFLAGS += -g -O

ansi: CFLAGS += -ansi
	\

c99: CFLAGS += -std=c99
	\

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