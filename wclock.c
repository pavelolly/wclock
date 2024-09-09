#include <stdlib.h>

#include "wclock.h"

void WClockDestroy(WClock *wclock) {
    free(wclock->sessions);
}