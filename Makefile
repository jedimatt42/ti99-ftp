GAS=tms9900-as
LD=tms9900-ld
CC=tms9900-gcc
LIBTI99?=/home/matthew/dev/github/jedimatt42/libti99
OBJCOPY=tms9900-objcopy
OBJDUMP=tms9900-objdump

FNAME=FORCEFTP
UCFNAME=$(shell echo -n $(FNAME) | tr 'a-z' 'A-Z')

LDFLAGS=\
  --script=linkfile -L$(LIBTI99) -lti99

CFLAGS=\
  -std=gnu99 -O2 -Werror --save-temp -I$(LIBTI99) -DBANK_STACK_SIZE=10

SRCS:=$(sort $(wildcard *.c) $(wildcard *.asm))

OBJECT_LIST:=$(SRCS:.c=.o)
OBJECT_LIST:=$(OBJECT_LIST:.asm=.o)

LINK_OBJECTS:=$(addprefix objects/,$(OBJECT_LIST))

all: $(FNAME).elf

$(FNAME).elf: $(OBJECT_LIST)
	$(LD) $(LINK_OBJECTS) $(LDFLAGS) -o $(FNAME).elf -Map=mapfile

.phony clean:
	rm -fr objects
	rm -f *.elf
	rm -f *.bin
	rm -f mapfile

%.o: %.asm
	mkdir -p objects
	cd objects; $(GAS) ../$< -o $@

%.o: %.c
	mkdir -p objects
	cd objects; $(CC) -c ../$< $(CFLAGS) -I/home/matthew/dev/gcc-9900/lib/gcc/tms9900/4.4.0/include -o $@

