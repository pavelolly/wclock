#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "wclock.h"


enum Subcommand {
    INVALID = -1,
    START,
    END,
    CLEAR,
    STATUS,
    HELP
};

void Usage();
enum Subcommand GetSubcommand(const char *literal);

int main(int argc, char *argv[]) {

    CStrs files = {0};
    DArrayAppend(&files, ".wclock");
    DArrayAppend(&files, ".wclock.old");

    char *cwd = GetCWD(NULL, 0);

    CStrs paths = FindAllFilesUp(cwd, &files);
    for (size_t i = 0; i < paths.count; ++i) {
        printf("%s\n", paths.items[i]);
    }
    free(cwd);
    return 0;

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

    bool couldLoad = WClockLoadFile(".wclock", &wclock);

    switch (subcommand) {
        case START:
            if (!wclock.lastSessionActive) {
                WClockStartSession(&wclock);
                
                printf("Started new session: %s\n", WClockTimeToString(WClockGetLastSession(&wclock).start));

                if (!WClockDumpFile(".wclock", &wclock)) {
                    printf("Failed to dump to .wclock\n");
                    return 1;
                }
            } else {
                printf("Session is already active, started at: %s\n", WClockTimeToString(WClockGetLastSession(&wclock).start));
            }


            break;
        case END:
            if (wclock.lastSessionActive) {
                WClockEndSession(&wclock);
                WClockSession lastSession = WClockGetLastSession(&wclock);
                printf("Ended session: %s : ", WClockTimeToString(lastSession.start));
                printf("%s\n",                 WClockTimeToString(lastSession.end));

                if (!WClockDumpFile(".wclock", &wclock)) {
                    printf("Failed to dump to .wclock\n");
                    return 1;
                }
            }
            else {
                printf("No active sessions: nothing to end\n");
            }

            break;
        case CLEAR:
            WClockClear(&wclock);
            if (!WClockDumpFile(".wclock", &wclock)) {
                printf("Failed to dump to .wclock\n");
                return 1;
            }

            break;
        case STATUS:
            if (couldLoad) {
                printf("Recorded %d sessions:\n", wclock.count);

                for (int i = 0; i < wclock.count; i++) {
                    WClockSession *s = wclock.sessions + i;
                    if (i < wclock.count - 1 || !wclock.lastSessionActive) {
                        printf("  %d. %s : ", i + 1, WClockTimeToString(s->start));
                        printf("%s\n",          WClockTimeToString(s->end)); 
                    }
                }
                printf("Current session: %s\n", wclock.lastSessionActive ? "active" : "not active");\
                if (wclock.lastSessionActive) {
                    printf("  %d: %s : ...\n", wclock.count, WClockTimeToString(WClockGetLastSession(&wclock).start));
                }
            } else {
                printf("No .wclock file in current directory\n");
            }

            break;
        default:
            assert(false && "Unreachable");
    }

    WClockDestroy(&wclock);

    printf("Done with no segfaults!\n");

    return 0;
}

void Usage() {
    printf("Usage: wclock <subcommand>\n");
    printf("Subcommands:\n");
    printf("  start     start new session\n");
    printf("  end       end current session\n");
    printf("  clear     clear information about sessions\n");
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
    if (!strcmp(literal, "clear")) {
        return CLEAR;
    }
    if (!strcmp(literal, "status")) {
        return STATUS;
    }
    if (!strcmp(literal, "help")) {
        return HELP;
    }
    return INVALID;
}