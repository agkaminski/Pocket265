GOAL = fw

CPU = 6502
TARGET = $(addsuffix .bin, ${GOAL})
GHASH = `git describe --always --dirty`
OBJ = main.o interrupt.o dl1414.o keyboard.o timer.o monitor.o

.PHONY: clean
.PRECIOUS: %.asm
MAKEFLAGS += --no-builtin-rules

all: ${TARGET}

pocket265.lib:
	@cd bsp && $(MAKE) all

${TARGET}: ${OBJ} pocket265.lib
	@ld65 -C bsp/pocket265.cfg -m fw.map -o "$@" ${OBJ} bsp/pocket265.lib

%.asm: %.c
	@cc65 -I bsp -o "$@" -t none -O --cpu ${CPU} "$<"

%.o: %.s
	@ca65 --cpu ${CPU} "$<" -o "$@"

%.o: %.asm
	@ca65 --cpu ${CPU} "$<" -o "$@"

clean:
	@rm -f *.o *.asm *.map *.bin
	@cd bsp && $(MAKE) clean
