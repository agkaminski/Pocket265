CPU = 6502

.PHONY: clean

all: pocket265.lib

pocket265.lib: supervision.lib crt0.S
	@ca65 --cpu ${CPU} crt0.S
	@cp supervision.lib pocket265.lib
	@ar65 a pocket265.lib crt0.o

clean:
	@rm -f pocket265.lib *.o
