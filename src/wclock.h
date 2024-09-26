#ifndef WCLOCK_H
#define WCLOCK_H

#include <time.h>
#include <stdint.h>

#undef bool
#undef false
#undef true

typedef uint8_t bool;
#define false 0
#define true  1

typedef unsigned int uint;

typedef struct {
    time_t start;
    time_t end;
} WClockSession;

typedef struct {
    // array of sessions
    WClockSession* sessions;
    int count;
    int capacity;

    // state
    uint8_t lastSessionActive;
} WClock;

void WClockStartSession(WClock *wclock);
void WClockEndSession(WClock *wclock);

WClockSession WClockGetLastSession(WClock *wclock);

char *WClockTimeToString(char *dest, size_t destSize, time_t time);

void WClockPrint(WClock *wclock);
void WClockDestroy(WClock *wclock);

// .wclock files dump and load
bool WClockDumpFile(const char *filename, WClock *wclock);
bool WClockLoadFile(const char *filename, WClock *wclock);

// file system operations
char **WClockFindAllFilesUp(const char *filename);

#endif // WCLOCK_H