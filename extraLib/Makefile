all: libjp.a

CPU             = msp430g2553
CFLAGS          = -mmcu=${CPU} -Os -I../h

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-as
AR              = msp430-elf-ar

OBJECTS         = stdjp.o

libjp.a: $(OBJECTS)
	$(AR) crs $@ $^

$(OBJECTS): stdjp.h

install: libjp.a
	mkdir -p ../h ../lib
	mv $^ ../lib
	cp *.h ../h

clean:
	rm -f *.a *.o *.elf
