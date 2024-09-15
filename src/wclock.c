#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "wclock.h"

void WClockStartSession(WClock *wclock) {
    if (wclock->lastSessionActive) {
        return;
    }

    wclock->lastSessionActive = 1;

    // append new session

    if (wclock->count == wclock->capacity) {
        wclock->sessions = (WClockSession *)realloc(wclock->sessions, wclock->capacity + sizeof(WClockSession));
        wclock->capacity += 1;
    }

    time(&wclock->sessions[wclock->count++].start);
}

void WClockEndSession(WClock *wclock) {
    if (!wclock->lastSessionActive) {
        return;
    }

    assert(wclock->count > 0 && "no last session");

    wclock->lastSessionActive = 0;

    time(&wclock->sessions[wclock->count - 1].end);
}

void WClockPrint(WClock *wclock) {
    printf("State: %d\n", wclock->lastSessionActive);
    for (int i = 0; i < wclock->count; i++) {
        printf("Session %d: %lld --- %lld\n", i, wclock->sessions[i].start, wclock->sessions[i].end);
    }
}

void WClockDestroy(WClock *wclock) {
    free(wclock->sessions);
}