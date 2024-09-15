include ../Makefile.env

WCLOCK_SOURCES = wclock.c wclock_format.c wclock_filesystem.c
WCLOCK_OBJECTS = wclock.o wclock_format.o wclock_filesystem.o

wclock.o: $(WCLOCK_SOURCES) wclock.h
	${CC} ${CFLAGS} ${DFLAGS} -I../ -c $(WCLOCK_SOURCES)
	${LD} -relocatable $(WCLOCK_OBJECTS) -o _wclock.o
	${RM} $(WCLOCK_OBJECTS)
	${MV} _wclock.o wclock.o

.PHONY: clean
clean:
	${RM} *.o *.exe
