
SRC_DIR = src

.PHONY: clean all

all:
	${MAKE} -C ${SRC_DIR}/

clean:
	${RM} -r *.o *.exe