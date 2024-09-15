include ../Makefile.env

darray.o: darray.c darray.h dstring.c dstring.h
	${CC} ${CFLAGS} ${DFLAGS} -c darray.c dstring.c
	${LD} -relocatable darray.o dstring.o -o _darray.o
	${RM} darray.o dstring.o
	${MV} _darray.o darray.o

.PHONY: clean
clean:
	${RM} *.o *.exe