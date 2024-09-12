CC = gcc
LD = ld
MV = mv
CFLAGS = --std=c99

wclock.exe: wclock.o main.c
	${CC} ${CFLAGS} ${DFLAGS} main.c wclock.o -o wclock.exe

WCLOCK_SOURCES = wclock.c wclock_format.c
WCLOCK_OBJECTS = wclock.o wclock_format.o

wclock.o: $(WCLOCK_SOURCES) wclock.h static_assert.h
	${CC} ${CFLAGS} ${DFLAGS} -c $(WCLOCK_SOURCES)
	${LD} -relocatable $(WCLOCK_OBJECTS) -o _wclock.o
	${RM} $(WCLOCK_OBJECTS)
	${MV} _wclock.o wclock.o
