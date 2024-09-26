include Makefile.var

SRC_DIR = src
UTILS_DIR = utils

wclock.exe: ${SRC_DIR}/wclock_cli.c ${SRC_DIR}/wclock.o ${UTILS_DIR}/darray.o
	${MAKE} -C utils/
	${MAKE} -C src/

	${CC} ${CFLAGS} ${DFLAGS} ${SRC_DIR}/wclock_cli.c ${SRC_DIR}/wclock.o ${UTILS_DIR}/darray.o -o wclock.exe

.PHONY: clean
clean:
	${RM} *.o *.exe
	${MAKE} -C utils/ clean
	${MAKE} -C src/   clean