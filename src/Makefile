include ../Makefile.var

WCLOCK_SOURCES = wclock_main.c wclock_format.c wclock_filesystem.c
WCLOCK_OBJECTS = wclock_main.o wclock_format.o wclock_filesystem.o

wclock.o: $(WCLOCK_SOURCES) wclock.h
	${CC} ${CFLAGS} ${DFLAGS} -I../ -c $(WCLOCK_SOURCES)
	${LD} -relocatable $(WCLOCK_OBJECTS) -o wclock.o

.PHONY: clean
clean:
	${RM} *.o *.exe
