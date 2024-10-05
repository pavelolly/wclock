
RM = rm -f
MV = mv
CP = cp

.PHONY: all clean

all:
	${MAKE} -C src/
	${CP} src/wclock.exe wclock.exe

clean:
	${RM} *.o *.exe
	${RM} src/*.o src/*.exe