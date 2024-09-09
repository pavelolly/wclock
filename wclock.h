#ifndef WCLOCK_H
#define WCLOCK_H

#include <time.h>
#include <stdbool.h>

typedef struct {
    time_t start;
    time_t end;
} WClockSession;

typedef struct {
    WClockSession* sessions;
    int numSessions;
} WClock;

void WClockDestroy(WClock *wclock);

bool WClockDumpFile(WClock *wclock, const char *filename);
bool WClockLoadFile(WClock *wclock, const char *filename);

#endif // WCLOCK_H