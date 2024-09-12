#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "static_assert.h"

#include "wclock.h"

enum { LITTLE_ENDIAN = 0, BIG_ENDIAN = 1 };

static inline int GetEndian(void) {
    static union {int i; char c;} u = {.i = 1};
    return u.c == 0; // 1 if big-endian
}

STATIC_ASSERT(sizeof(time_t) == 8 && "time_t not being 8 bytes is not handled");

static time_t ReverseBytes(time_t t) {
    const static time_t byte = 0xff;
    return ((t & (byte << 0*8)) << (7*8)) |
           ((t & (byte << 1*8)) << (5*8)) |
           ((t & (byte << 2*8)) << (3*8)) |
           ((t & (byte << 3*8)) << (1*8)) |

           ((t & (byte << 4*8)) >> (1*8)) |
           ((t & (byte << 5*8)) >> (3*8)) |
           ((t & (byte << 6*8)) >> (5*8)) |
           ((t & (byte << 7*8)) >> (7*8));
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
        DEBUG_LOG("[FileSize]: pos < 0 (%d)\n", pos);
        return -1;
    }
    if (fseek(file, 0, SEEK_END)) {
        DEBUG_LOG("[FileSize]: Could not seek for SEEK_END\n");
        return -1;
    }
    long size = ftell(file);
    if (size < 0) {
        DEBUG_LOG("[FileSize]: size < 0 (%d)\n", size);
        return -1;
    }
    if (fseek(file, pos, SEEK_SET)) {
        DEBUG_LOG("[FileSize]: Could not seek for SEEK_SET + pos\n");
        return -1;
    }
    return size - pos;
}

// header bytes for .wclock files
#define WF_FIRST_BYTE_LE 0xc1u // LITTLE-ENDIAN
#define WF_FIRST_BYTE_BE 0xcbu // BIG-ENDIAN
#define WF_SECOND_BYTE   0x0cu

bool WClockDumpFile(WClock *wclock, const char *filename)  {
    if (!wclock) {
        DEBUG_LOG("[DUMP]: wclock is NULL\n");
        return false;
    }

    FILE *file = fopen(filename, "wb");
    bool returnValue = true;
    
    if (!file) {
        DEBUG_LOG("[DUMP]: Failed to open '%s'\n", filename);

        returnValue = false;
        goto defer;
    }

    uint8_t headerBytes[2] = {
        GetEndian() == LITTLE_ENDIAN ? WF_FIRST_BYTE_LE : WF_FIRST_BYTE_BE,
        WF_SECOND_BYTE
    };

    // write header bytes

    if (fwrite(headerBytes, 1, 2, file) != 2) {
        DEBUG_LOG("[DUMP]: Could not write first two bytes\n");

        returnValue = false;
        goto defer;
    }

    // write state

    unsigned char byte = wclock->lastSessionActive;
    if (fwrite(&byte, 1, 1, file) < 1) {
        DEBUG_LOG("[DUMP]: Could not write state\n");

        returnValue = false;
        goto defer;
    }

    // write sessions

    if (fwrite(wclock->sessions, sizeof(WClockSession), wclock->numSessions, file) < wclock->numSessions) {
        DEBUG_LOG("[DUMP]: Could not write all the sessions\n");

        returnValue = false;
        goto defer;
    }

defer:
    fclose(file);
    return returnValue;
}

bool WClockLoadFile(WClock *wclock, const char *filename) {
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

    // read header bytes

    uint8_t headerBytes[2];
    if (fread(headerBytes, 1, 2, file) < 2) {
        DEBUG_LOG("[LOAD]: Failed to read first two bytes\n");

        returnValue = false;
        goto defer;
    }

    // read state

    unsigned char byte;
    if (fread(&byte, 1, 1, file) < 1) {
        DEBUG_LOG("[LOAD]: Failed to read state\n");

        returnValue = false;
        goto defer;
    }

    // read sessions
    
    int endian = GetEndian();
    int fileEndian;
    if (headerBytes[0] == WF_FIRST_BYTE_BE) {
        fileEndian = BIG_ENDIAN;
    } else if (headerBytes[0] == WF_FIRST_BYTE_LE) {
        fileEndian = LITTLE_ENDIAN;
    } else {
        DEBUG_LOG("[LOAD]: First byte is neither 0xc1 nor 0xcb\n");

        returnValue = false;
        goto defer;
    }
    if (headerBytes[1] != WF_SECOND_BYTE) {
        DEBUG_LOG("[LOAD]: Second byte is not 0x0c\n");

        returnValue = false;
        goto defer;
    }

    bool reverseBytes = endian != fileEndian;
    long bytesLeft = FileSize(file);
    if (bytesLeft < 0) {
        DEBUG_LOG("[LOAD]: Failed to determine size of file (FileSize returned %ld)\n", bytesLeft);

        returnValue = false;
        goto defer;
    }

    if (bytesLeft % sizeof(WClockSession) != 0) {
        DEBUG_LOG("[LOAD]: Number of bytes in file is not divisible by sizeof(WClockSession)\n");

        returnValue = false;
        goto defer;
    }

    int numSessions = bytesLeft / sizeof(WClockSession);
    WClockSession *sessions = malloc(numSessions * sizeof(WClockSession));
    if (fread(sessions, sizeof(WClockSession), numSessions, file) < numSessions) {
        DEBUG_LOG("[LOAD]: Failed to read bytes for sessions\n");

        free(sessions);
        returnValue = false;
        goto defer;
    }

    if (reverseBytes) {
        for (int i = 0; i < numSessions; i++) {
            sessions[i].start = ReverseBytes(sessions[i].start);
            sessions[i].end   = ReverseBytes(sessions[i].end);
        }
    }

    wclock->sessions    = sessions;
    wclock->numSessions = numSessions;
    wclock->lastSessionActive = byte;
defer:
    fclose(file);
    return returnValue;
}

