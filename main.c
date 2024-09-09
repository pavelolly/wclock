#include <stdio.h>

#include "wclock.h"


int main(void) {
    WClockSession sessions[] = {
        {.start = 1,   .end = 10},
        {.start = 11,  .end = 110},
        {.start = 120, .end = 5000}
    };
    WClock wclock = {
        .sessions = sessions,
        .numSessions = sizeof(sessions)/sizeof(*sessions)
    };

    if (!WClockDumpFile(&wclock, ".wclock")) {
        printf("Failed to dump to .wclock\n");
        return 1;
    }

    if (!WClockLoadFile(&wclock, ".wclock")) {
        printf("Falied to load from .wclock\n");
        return 1;
    }

    for (int i = 0; i < wclock.numSessions; i++) {
        printf("Session %d: %lld : %lld\n", i, wclock.sessions[i].start, wclock.sessions[i].end);
    }

    WClockDestroy(&wclock);

    return 0;
}