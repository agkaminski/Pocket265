EXAMPLES = hello fibonacci fibonacci-big counter clock clock-bcd cpuid nmi guess_game

TARGET = $(addsuffix .txt, ${EXAMPLES})

.PHONY: clean
.PRECIOUS: %.bin

%.txt: %.bin
	@xxd -g1 -c8 -o0x800 "$<" > "$@"

%.bin: %.hex
	@objcopy -Iihex "$<" -Obinary "$@"

%.hex: %.crasm
	crasm "$<" -o "$@" >> build.log

all: ${TARGET}

clean:
	@rm -f *.hex *.bin *.log *.txt
