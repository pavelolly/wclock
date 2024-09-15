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
    INFO,
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
    if (!strcmp(literal, "info")) {
        return INFO;
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
    char **wclockPaths = WClockFindAllFiles(".wclock");

    for (int i = 0; wclockPaths[i]; i++) {
        printf("Path: '%s'\n", wclockPaths[i]);
    }

    switch (subcommand) {
        case START:
            break;
        case END:
            break;
        case STATUS:
            break;
        case INFO:
            break;
        default:
            assert(false && "Unreachable");
    }

    for (int i = 0; wclockPaths[i]; i++) {
        free(wclockPaths[i]);
    }
    free(wclockPaths);

    printf("Done!\n");

    return 0;
}