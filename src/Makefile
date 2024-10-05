
CC = gcc --std=c99
RM = rm -f

CFLAGS = -ggdb -Wall -Wextra -Werror

OBJECTS = darray.o dstring.o wclock_cli.o wclock_core.o wclock_filesystem.o wclock_format.o

wclock.exe: ${OBJECTS}
	${CC} -o wclock.exe ${OBJECTS}


darray.o: darray.c darray.h 
dstring.o: dstring.c darray.h dstring.h
wclock_cli.o: wclock_cli.c wclock.h
wclock_core.o: wclock_core.c wclock.h
wclock_filesystem.o: wclock_filesystem.c wclock.h darray.h dstring.h dirent.h
wclock_format.o: wclock_format.c wclock.h