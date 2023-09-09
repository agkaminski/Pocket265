CC := gcc
CFLAGS := -g -Wall -Werror -Wextra -Wno-type-limits -O2 -ansi -std=gnu99
DEBUG := -DNDEBUG

OUT = pocket265-sim
OBJ = pocket265.o
LIB = simak65

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -I. -c -o $@ $<

$(OUT): $(OBJ)
	$(CC) -o $@ $^ -l $(LIB)

all: $(OUT)

clean:
	rm -f *.o $(OUT)

.PHONY: clean
.PHONY: install
