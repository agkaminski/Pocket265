GOAL = fw

CPU = 6502
TARGET = $(addsuffix .bin, ${GOAL})
GHASH = `git describe --always --dirty`
OBJ = main.o

.PHONY: clean

all: ${TARGET}

pocket265.lib:
	@cd bsp && $(MAKE) all

${TARGET}: ${OBJ} pocket265.lib
	@ld65 -C bsp/pocket265.cfg -m main.map -o "$@" ${OBJ} bsp/pocket265.lib

%.s: %.c
	@cc65 -t none -O --cpu ${CPU} "$<"

%.o: %.s
	@ca65 --cpu ${CPU} "$<" -o "$@"

clean:
	@rm -f *.hex *.bin *.log *.map
	@cd bsp && $(MAKE) clean
