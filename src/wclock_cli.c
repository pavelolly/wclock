#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "wclock.h"


enum Subcommand {
    INVALID = -1,
    START,
    END,
    STATUS,
    HELP
};

void Usage();
enum Subcommand GetSubcommand(const char *literal);

#define TIME_TEXT_BUFFER_SIZE 64

int main(int argc, char *argv[]) {
    if (argc < 2) {
        Usage();
        return 1;
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

    // Used for printing time
    char timeTextBuffer[TIME_TEXT_BUFFER_SIZE];

    bool couldLoad = WClockLoadFile(".wclock", &wclock);

    switch (subcommand) {
        case START:
            if (!wclock.lastSessionActive) {
                WClockStartSession(&wclock);
                
                printf("Started new session: %s\n", WClockTimeToString(timeTextBuffer, TIME_TEXT_BUFFER_SIZE, WClockGetLastSession(&wclock).start));

                if (!WClockDumpFile(".wclock", &wclock)) {
                    printf("Failed to dump to .wclock\n");
                    return 1;
                }
            } else {
                printf("Session is already active, started at: %s\n", WClockTimeToString(timeTextBuffer, TIME_TEXT_BUFFER_SIZE, WClockGetLastSession(&wclock).start));
            }


            break;
        case END:
            if (wclock.lastSessionActive) {
                WClockEndSession(&wclock);
                WClockSession lastSession = WClockGetLastSession(&wclock);
                printf("Ended session: %s : ", WClockTimeToString(timeTextBuffer, TIME_TEXT_BUFFER_SIZE, lastSession.start));
                printf("%s\n",                 WClockTimeToString(timeTextBuffer, TIME_TEXT_BUFFER_SIZE, lastSession.end));

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
                        printf("  %d. %s : ", i + 1, WClockTimeToString(timeTextBuffer, TIME_TEXT_BUFFER_SIZE, s->start));
                        printf("%s\n",          WClockTimeToString(timeTextBuffer, TIME_TEXT_BUFFER_SIZE, s->end)); 
                    }
                }
                printf("Current session: %s\n", wclock.lastSessionActive ? "active" : "not active");\
                if (wclock.lastSessionActive) {
                    printf("  %d: %s : ...\n", wclock.count, WClockTimeToString(timeTextBuffer, TIME_TEXT_BUFFER_SIZE, WClockGetLastSession(&wclock).start));
                }
            } else {
                printf("No .wclock file in current directory\n");
            }

            break;
        default:
            assert(false && "Unreachable");
    }

    printf("Done with no segfaults!\n");

    return 0;
}

void Usage() {
    printf("Usage: wclock <subcommand>\n");
    printf("Subcommands:\n");
    printf("  start     start new session\n");
    printf("  end       end current session\n");
    printf("  status    show status of current session\n");
    printf("\n");
    printf("  help      show this page\n");
    printf("\n");
}

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