CPU = 6502

# Split syscalls in separete files, so not used ones are linked out
SYSCALLS = $(addprefix syscall/svc_, irqRegister nmiRegister putch puts putb \
                                     printxU16 printdU16 printdS16 scanxU16 \
                                     scandU16 getKey getKeyNb getJiffies \
                                     getSeconds msleep sleep nmiDisable \
                                     nmiStart)
SYSCALLS_OBJ = ${addsuffix .o, ${SYSCALLS}}

.PHONY: clean

all: pocket265.lib

%.o: %.S
	@ca65 --cpu ${CPU} "$<" -o "$@"

pocket265.lib: none.lib crt0.o ${SYSCALLS_OBJ}
	@cp none.lib pocket265.lib
	@ar65 a pocket265.lib crt0.o ${SYSCALLS_OBJ}

clean:
	@rm -f pocket265.lib *.o
