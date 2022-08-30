GOAL = fw

TARGET = $(addsuffix .bin, ${GOAL})
GHASH = `git describe --always --dirty`

.PHONY: clean
.PRECIOUS: %.hex

%.bin: %.hex
	@objcopy --gap-fill 0xFF -Iihex "$<" -Obinary "$@"

%.hex: %.crasm
	@echo "ASC '${GHASH}'" > version.inc
	@crasm "$<" -o "$@" > build.log

all: ${TARGET}

clean:
	@rm -f *.hex *.bin *.log
