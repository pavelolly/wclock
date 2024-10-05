#ifndef WCLOCK_H
#define WCLOCK_H

#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#include "darray.h"

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
void WClockClear(WClock *wclock);

WClockSession WClockGetLastSession(WClock *wclock);

const char *WClockTimeToString(time_t time);

void WClockPrint(WClock *wclock);
void WClockDestroy(WClock *wclock);

// .wclock files dump and load
bool WClockDumpFile(const char *filename, WClock *wclock);
bool WClockLoadFile(const char *filename, WClock *wclock);

// file system operations
#ifdef _WIN32
    #include <direct.h>
    #define GetCWD _getcwd
#else
    #include <unistd.h>
    #define GetCWD getcwd
#endif


INSTANTIATE_DARRAY(CStrs, char *);
void CStrsFree(CStrs *cstrs);

CStrs GetFiles(const char *dirPath);
bool PathExists(const char *path);
bool IsDir(const char *path);
bool IsFile(const char *path);
const char *CutPath(const char *path);
CStrs FindAllFilesUp(const char *dirPath, const CStrs *filesList);


#endif // WCLOCK_H