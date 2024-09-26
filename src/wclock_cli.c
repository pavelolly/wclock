#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "wclock.h"

void Usage() {
    printf("Usage: wclock <subcommand> [options]...\n");
    printf("Subcommands:\n");
    printf("  start     start new session\n");
    printf("  end       end current session\n");
    printf("  status    show status of current session\n");
    printf("  info      show information about all sessions\n");
    printf("\n");
    printf("  help      show this page\n");
    printf("\n");
}

enum Subcommand {
    INVALID = -1,
    START,
    END,
    STATUS,
    HELP
};

enum Subcommand GetSubcommand(const char *literal) {
    if (!strcmp(literal, "start")) {
        return START;
    }
    if (!strcmp(literal, "end")) {
        return END;
    }
    if (!strcmp(literal, "status")) {
        return STATUS;
    }
    if (!strcmp(literal, "help")) {
        return HELP;
    }
    return INVALID;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        Usage();
        return 0;
    }

    enum Subcommand subcommand = GetSubcommand(argv[1]);
        
    if (subcommand == INVALID) {
        printf("ERROR: Unknowm subcommand '%s'\n", argv[1]);
        Usage();
        return 1;
    }
    if (subcommand == HELP) {
        Usage();
        return 0;
    }

    WClock wclock = {0};
    // char **wclockPaths = WClockFindAllFilesUp(".wclock");

    // printf("Found .wclock files:\n");
    // for (int i = 0; wclockPaths[i]; i++) {
    //     printf("'%s'\n", wclockPaths[i]);
    // }

    bool couldLoad = WClockLoadFile(".wclock", &wclock);

    switch (subcommand) {
        case START:
            if (!wclock.lastSessionActive) {
                WClockStartSession(&wclock);
                printf("Started new session: %lld\n", WClockGetLastSession(&wclock).start);

                if (!WClockDumpFile(".wclock", &wclock)) {
                    printf("Failed to dump to .wclock\n");
                    return 1;
                }
            } else {
                printf("Session is already active, started at: %lld\n", WClockGetLastSession(&wclock).start);
            }


            break;
        case END:
            if (wclock.lastSessionActive) {
                WClockEndSession(&wclock);
                WClockSession s = WClockGetLastSession(&wclock);
                printf("Ended session: %lld : %lld\n", s.start, s.end);

                if (!WClockDumpFile(".wclock", &wclock)) {
                    printf("Failed to dump to .wclock\n");
                    return 1;
                }
            }
            else {
                printf("No active sessions: nothing to end\n");
            }

            break;
        case STATUS:
            if (couldLoad) {
                printf("Recorded %d sessions:\n", wclock.count);

                for (int i = 0; i < wclock.count; i++) {
                    WClockSession *s = wclock.sessions + i;
                    if (i < wclock.count - 1 || !wclock.lastSessionActive) {
                        printf("  %d: %lld : %lld\n", i + 1, s->start, s->end);    
                    }
                }
                printf("Current session: %s\n", wclock.lastSessionActive ? "active" : "not active");\
                if (wclock.lastSessionActive) {
                    printf("  %d: %lld : ...\n", wclock.count, WClockGetLastSession(&wclock).start);
                }
            } else {
                printf("No .wclock file in current directory\n");
            }

            break;
        default:
            assert(false && "Unreachable");
    }

    // for (int i = 0; wclockPaths[i]; i++) {
    //     free(wclockPaths[i]);
    // }
    // free(wclockPaths);

    printf("Done with no segfaults!\n");

    return 0;
}