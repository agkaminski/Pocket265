GOAL = hello

CPU = 6502
TARGET_BIN = $(addsuffix .bin, ${GOAL})
TARGET_TXT = $(addsuffix .txt, ${GOAL})
OBJ = hello.o
BSP = ../bsp/

.PHONY: clean
.PRECIOUS: %.bin %.asm
MAKEFLAGS += --no-builtin-rules

all: ${TARGET_BIN} ${TARGET_TXT}

pocket265.lib:
	@cd ${BSP} && $(MAKE) all

${TARGET_BIN}: ${OBJ} pocket265.lib
	@ld65 -C ${BSP}pocket265-user.cfg -m ${GOAL}.map -o "$@" ${OBJ} ${BSP}pocket265.lib

%.txt: %.bin
	@xxd -g1 -c8 -o0x800 "$<" > "$@"

%.asm: %.c
	@cc65 -I ${BSP} -o "$@" -t none -O --cpu ${CPU} "$<"

%.o: %.s
	@ca65 --cpu ${CPU} "$<" -o "$@"

%.o: %.asm
	@ca65 --cpu ${CPU} "$<" -o "$@"

clean:
	@rm -f *.o *.asm *.map *.bin *.txt
	@cd ${BSP} && $(MAKE) clean
