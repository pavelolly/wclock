#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "wclock.h"

enum { LITTLE_ENDIAN = 0, BIG_ENDIAN = 1 };

static inline int GetEndian(void) {
    static union {int i; char c;} u = {.i = 1};
    return u.c == 0; // true means BIG_ENDIAN; false means LITTLE_ENDIAN
}

static time_t ReverseBytes(time_t t) {
    static const time_t byte = 0xff;
    if (sizeof(time_t) == 8) {
        return ((t & (byte << 0*8)) << (7*8)) |
               ((t & (byte << 1*8)) << (5*8)) |
               ((t & (byte << 2*8)) << (3*8)) |
               ((t & (byte << 3*8)) << (1*8)) |

               ((t & (byte << 4*8)) >> (1*8)) |
               ((t & (byte << 5*8)) >> (3*8)) |
               ((t & (byte << 6*8)) >> (5*8)) |
               ((t & (byte << 7*8)) >> (7*8));
    } else if (sizeof(time_t) == 4) {
        return ((t & (byte << 0*8)) << (3*8)) |
               ((t & (byte << 1*8)) << (1*8)) |

               ((t & (byte << 2*8)) >> (1*8)) |
               ((t & (byte << 3*8)) >> (3*8));
    }

    assert(false && "sizeof(time_t) is neither 4 nor 8");
}

#ifdef DEBUG
    #define DEBUG_LOG(...) printf(__VA_ARGS__)
#else
    #define DEBUG_LOG(...)
#endif

// returns number of bytes from the current file position to the end of file
static long FileSize(FILE *file) {
    long pos = ftell(file);
    if (pos < 0) {
        return -1;
    }
    if (fseek(file, 0, SEEK_END)) {
        return -1;
    }
    long size = ftell(file);
    if (size < 0) {
        return -1;
    }
    if (fseek(file, pos, SEEK_SET)) {
        return -1;
    }
    return size - pos;
}

// .wclock format version
#define WF_VERSION_MAJOR ((unsigned char)0)
#define WF_VERSION_MINOR ((unsigned char)0)

// .wclock header bytes
#define WF_FIRST_BYTE_LE ((unsigned char)0xc1u) // LITTLE-ENDIAN
#define WF_FIRST_BYTE_BE ((unsigned char)0xcbu) // BIG-ENDIAN
#define WF_SECOND_BYTE_4 ((unsigned char)0x4cu) // sizeof(time_t) == 4
#define WF_SECOND_BYTE_8 ((unsigned char)0x8cu) // sizeof(time_t) == 8

struct WClockSystemInfo {
    uint8_t endian;
    uint8_t sizeofTime;
};

static bool WClockDumpSystemInfo(struct WClockSystemInfo *sys, FILE *file) {
    if (sys->endian == LITTLE_ENDIAN) {
        sys->endian = WF_FIRST_BYTE_LE;
    } else if (sys->endian == BIG_ENDIAN) {
        sys->endian = WF_FIRST_BYTE_BE;
    } else {
        return false;
    }

    if (sys->sizeofTime == 4) {
        sys->sizeofTime = WF_SECOND_BYTE_4;
    } else if (sys->sizeofTime == 8) {
        sys->sizeofTime = WF_SECOND_BYTE_8;
    } else {
        return false;
    }

    if (fwrite(sys, 1, 2, file) != 2) {
        return false;
    }
    
    return true;
}

static bool WClockLoadSystemInfo(struct WClockSystemInfo *sys, FILE *file) {
    struct WClockSystemInfo tmp;
    if (fread(&tmp, 1, 2, file) != 2) {
        return false;
    }

    if (tmp.endian == WF_FIRST_BYTE_LE) {
        tmp.endian = LITTLE_ENDIAN;
    } else if (tmp.endian == WF_FIRST_BYTE_BE) {
        tmp.endian = BIG_ENDIAN;
    } else {
        return false;
    }

    if (tmp.sizeofTime == WF_SECOND_BYTE_4) {
        tmp.sizeofTime = 4;
    } else if (tmp.sizeofTime == WF_SECOND_BYTE_8) {
        tmp.sizeofTime = 8;
    } else {
        return false;
    }

    sys->endian = tmp.endian;
    sys->sizeofTime = tmp.sizeofTime;
    return true;
}

struct WClockVersionInfo {
    uint8_t major;
    uint8_t minor;
};

static bool WClockDumpVersionInfo(struct WClockVersionInfo *ver, FILE* file) {
    return fwrite(ver, 1, 2, file) == 2;
}

static bool WClockLoadVersionInfo(struct WClockVersionInfo *ver, FILE *file) {
    struct WClockVersionInfo tmp;
    if (fread(&tmp, 1, 2, file) != 2) {
        return false;
    }

    ver->major = tmp.major;
    ver->minor = tmp.minor;
    return true;
}

static bool WClockDumpWClock(WClock *wclock, FILE *file) {
    if (fwrite(&wclock->lastSessionActive, 1, 1, file) != 1) {
        return false;
    }

    if (fwrite(wclock->sessions, sizeof(WClockSession), wclock->count, file) != (size_t)wclock->count) {
        return false;
    }

    return true;
}

static bool WClockLoadWclock(WClock *wclock, struct WClockSystemInfo *sys, [[maybe_unused]] struct WClockVersionInfo *ver, FILE *file) {
    WClock tmp;

    bool differentEndian     = GetEndian() != sys->endian;
    bool differentSizeofTime = sizeof(time_t) != sys->sizeofTime;

    if (fread(&tmp.lastSessionActive, 1, 1, file) != 1) {
        return false;
    }

    long bytesLeft = FileSize(file);
    if (bytesLeft % (sys->sizeofTime * 2) != 0) {
        return false;
    }
    int countSessions = bytesLeft / (sys->sizeofTime * 2);

    // allocate memeory for countSessions + 1 elements because it is very likely that you will create new session in future
    tmp.sessions = (WClockSession *)malloc((countSessions + 1) * sizeof(WClockSession));
    tmp.count    = countSessions;
    tmp.capacity = countSessions + 1;

    uint64_t buf[2]; // you need at most 2 * 8 bytes per session
    for (int i = 0; i < tmp.count; i++) {
        if (fread(buf, sys->sizeofTime, 2, file) != 2) {
            free(tmp.sessions);
            return false;
        }
        if (differentSizeofTime && sys->sizeofTime < sizeof(time_t)) {
            // split buf[0] into 2 parts and share them between buf[0] and buf[1]
            buf[1] = buf[0] & 0xffffffff;
            buf[0] = buf[0] >> 32; 
        }
        tmp.sessions[i].start = buf[0];
        tmp.sessions[i].end   = buf[1];

        if (differentEndian) {
            tmp.sessions[i].start = ReverseBytes(tmp.sessions[i].start);
            tmp.sessions[i].end   = ReverseBytes(tmp.sessions[i].end);
        }
    }

    WClockDestroy(wclock);
    memcpy(wclock, &tmp, sizeof(*wclock));
    return true;
}

bool WClockDumpFile(const char *filename, WClock *wclock)  {
    if (!wclock) {
        DEBUG_LOG("[DUMP]: wclock is NULL\n");
        return false;
    }

    FILE *file = fopen(filename, "wb");
    bool returnValue = true;
    
    if (!file) {
        DEBUG_LOG("[DUMP]: Could not open '%s'\n", filename);

        returnValue = false;
        goto defer;
    }

    // write system info

    struct WClockSystemInfo sys = {
        .endian     = GetEndian(),
        .sizeofTime = sizeof(time_t)
    };

    if (!WClockDumpSystemInfo(&sys, file)) {
        DEBUG_LOG("[DUMP] Failed to dump system info\n");

        returnValue = false;
        goto defer;
    }

    // write version

    struct WClockVersionInfo ver = {
        .major = WF_VERSION_MAJOR,
        .minor = WF_VERSION_MINOR
    };

    if (!WClockDumpVersionInfo(&ver, file)) {
        DEBUG_LOG("[DUMP] Failed to dump version info\n");

        returnValue = false;
        goto defer;
    }

    // write wclock itself

    if (!WClockDumpWClock(wclock, file)) {
        DEBUG_LOG("[DUMP] Failed to dump wclock body\n");

        returnValue = false;
        goto defer;
    }

defer:
    fclose(file);
    return returnValue;
}

bool WClockLoadFile(const char *filename, WClock *wclock) {
    if (!wclock) {
        DEBUG_LOG("[LOAD]: wclock is NULL\n");
        return false;
    }

    FILE *file = fopen(filename, "rb");
    bool returnValue = true;
    if (!file) {
        DEBUG_LOG("[LOAD]: Could not open '%s'\n", filename);

        returnValue = false;
        goto defer;
    }

    // read system info

    struct WClockSystemInfo sys;
    if (!WClockLoadSystemInfo(&sys, file)) {
        DEBUG_LOG("[LOAD] Failed to load system info\n");

        returnValue = false;
        goto defer;
    }

    // read version

    struct WClockVersionInfo ver;
    if (!WClockLoadVersionInfo(&ver, file)) {
        DEBUG_LOG("[LOAD] Failed to load version info\n");

        returnValue = false;
        goto defer;
    }

    // read wclock itself

    if (!WClockLoadWclock(wclock, &sys, &ver, file)) {
        DEBUG_LOG("[LOAD] Failed to load wclock body\n");

        returnValue = false;
        goto defer;
    }

defer:
    fclose(file);
    return returnValue;
}

