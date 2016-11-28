all: librand.a randDemo.elf

CPU             = msp430g2553
CFLAGS          = -mmcu=${CPU} -Os -I../h

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-as
AR              = msp430-elf-ar

OBJECTS         = rand.o

librand.a: $(OBJECTS)
	$(AR) crs $@ $^

$(OBJECTS): rand.h

install: librand.a
	mkdir -p ../h ../lib
	mv $^ ../lib
	cp *.h ../h

randDemo.elf: randDemo.o librand.a
	$(CC) $(CFLAGS) $^ -L../lib -lTimer -lLcd -o $@

load: randDemo.elf
	mspdebug rf2500 "prog $^"

clean:
	rm -f *.a *.o *.elf
