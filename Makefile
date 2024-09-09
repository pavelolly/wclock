CC = gcc
LD = ld
MV = mv
CFLAGS = --std=c99

wclock.exe: wclock.o main.c
	${CC} ${CFLAGS} ${DFLAGS} main.c wclock.o -o wclock.exe

wclock.o: wclock.c wclock_format.c
	${CC} ${CFLAGS} ${DFLAGS} -c wclock.c wclock_format.c
	${LD} -relocatable wclock.o wclock_format.o -o _wclock.o
	${RM} wclock.o wclock_format.o
	${MV} _wclock.o wclock.o
