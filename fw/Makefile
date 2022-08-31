GOAL = fw

CPU = 6502
TARGET = $(addsuffix .bin, ${GOAL})
GHASH = `git describe --always --dirty`
OBJ = main.o

.PHONY: clean
.SUFFIXES:

all: ${TARGET}

pocket265.lib:
	@cd bsp && $(MAKE) all

${TARGET}: ${OBJ} pocket265.lib
	@ld65 -C bsp/pocket265.cfg -m main.map -o "$@" ${OBJ} bsp/pocket265.lib

%.asm: %.c
	cc65 -o "$@" -t none -O --cpu ${CPU} "$<"

%.o: %.s
	@ca65 --cpu ${CPU} "$<" -o "$@"

%.o: %.asm
	@ca65 --cpu ${CPU} "$<" -o "$@"

clean:
	@rm -f *.o *.asm *.map
	@cd bsp && $(MAKE) clean
