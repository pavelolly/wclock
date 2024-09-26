
CC = gcc
RM = rm -f

CFLAGS = --std=c99 -Wall -Wextra -ggdb

SOURCES = darray.c dstring.c wclock_cli.c wclock_core.c wclock_filesystem.c wclock_format.c
HEADERS = darray.h dstring.h minirent.h static_assert.h wclock.h

../wclock.exe: ${SOURCES} ${HEADERS}
	${CC} ${CFLAGS} -o ../wclock.exe ${SOURCES}