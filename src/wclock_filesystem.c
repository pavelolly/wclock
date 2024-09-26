#include <stdlib.h>

#include <direct.h>
#define PATH_SEP "\\"
#define PATH_SEP_CHAR '\\'

#define MINIRENT_IMPLEMENTATION
#include <utils/minirent.h>

#include <utils/darray.h>
#include <utils/dstring.h>

#include "wclock.h"

INSTANTIATE_DARRAY(Cstrs, char *);

char **WClockFindAllFilesUp(const char *filename) {
    char *cwd = _getcwd(NULL, 0);

    Cstrs paths = {0};

    char *lsep, *rsep;
    do {
        // printf("Exploring path: '%s'\n", cwd);

        DIR *dir = opendir(cwd);

        struct dirent *entry;
        while ((entry = readdir(dir))) {
            if (!strcmp(entry->d_name, filename)) {
                DString filenameAbs = {0};
                DStringExtend(&filenameAbs, cwd);
                if (cwd[strlen(cwd) - 1] != PATH_SEP_CHAR) {
                    DStringAppend(&filenameAbs, PATH_SEP_CHAR);
                }
                DStringExtend(&filenameAbs, entry->d_name);

                DArrayAppend(&paths, filenameAbs.items);
            }
        }

        if (cwd[strlen(cwd) - 1] == PATH_SEP_CHAR) {
            break;
        }

        lsep = strchr(cwd, PATH_SEP_CHAR);
        rsep = strrchr(cwd, PATH_SEP_CHAR);
        if (lsep != rsep || (rsep && cwd[strlen(cwd) - 1] != PATH_SEP_CHAR)) {
            *rsep = '\0';
        }

        free(dir->dirent);
        free(dir);
    } while (lsep);

    DArrayAppend(&paths, NULL);

    free(cwd);

    return paths.items;
}