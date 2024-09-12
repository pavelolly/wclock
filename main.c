#include <stdio.h>
#include <windows.h>

#include "wclock.h"


int main(void) {
    WClock wclock = {0};
    WClockLoadFile(".wclock", &wclock);

    WClockStartSession(&wclock);
    Sleep(5000);
    WClockEndSession(&wclock);

    WClockDumpFile(".wclock", &wclock);

    WClockDestroy(&wclock);

    return 0;
}