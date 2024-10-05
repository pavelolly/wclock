#include <stdlib.h>

#include "darray.h"
#include "dstring.h"

#include "wclock.h"

#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h> // _getcwd
    #include "dirent.h" // opendir, readdir, closedir
    #include <windef.h> // MAX_PATH
    #define GetCWD _getcwd
#else
    #include <unistd.h> // getcwd
    #include <dirent.h> // opendir, readdir, closedir
    #include <linux/limits.h> // PATH_MAX
    #define MAX_PATH PATH_MAX
    #define GetCWD getcwd
#endif

void CStrsFree(CStrs *cstrs) {
    for (size_t i = 0; i < cstrs->count; ++i) {
        free(cstrs->items[i]);
    }
    DArrayFree(cstrs);
}

CStrs GetFiles(const char *dirPath) {
    CStrs paths = {0};
    DIR *dirp = opendir(dirPath);
    if (dirp) {
        struct dirent *entry;
        while ((entry = readdir(dirp))) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            char *newPath = malloc(MAX_PATH + 1);
            #ifdef _WIN32
                snprintf(newPath, MAX_PATH, "%s\\%s", dirPath, entry->d_name);
            #else
                snprintf(newPath, MAX_PATH, "%s/%s", dirPath, entry->d_name);
            #endif

            DArrayAppend(&paths, newPath);
        }
    }

    return paths;
}

bool PathExists(const char *path) {
    struct stat statBuffer = {0};
    return stat(path, &statBuffer) == 0;
}

bool IsDir(const char *path) {
    struct stat statBuffer = {0};
    if (stat(path, &statBuffer) == 0) {
        return statBuffer.st_mode & S_IFDIR;
    }
    return false;
}

bool IsFile(const char *path) {
    struct stat statBuffer = {0};
    if (stat(path, &statBuffer) == 0) {
        return statBuffer.st_mode & S_IFREG;
    }
    return false;
}

const char *CutPath(const char *dirPath) {
    static char prevPath[MAX_PATH] = {0};
    memset(prevPath, 0, MAX_PATH);

    int len = (int)strlen(dirPath);
    strcpy(prevPath, dirPath);

    for (int i = len - 1; i >= 0; --i) {
        if (prevPath[i] == '\\' || prevPath[i] == '/') {
            if (i == len - 1) continue;
            prevPath[i] = '\0';
            break;
        }
        if (i == 0) {
            prevPath[i] = '\0';
        }
    }

    return prevPath;
}

const char *GetFilename(const char *path) {
    static char filename[MAX_PATH];
    memset(filename, 0, MAX_PATH);
    
    int len = (int)strlen(path);
    const char *begin = path;
    const char *end = path + len;
    for (int i = len - 1; i >= 0; --i) {
        if (path[i] == '\\' || path[i] == '/') {
            if (i == len - 1) {
                end--;
            } else {
                begin = path + i + 1;
                break;
            }
        }
    }

    return strncpy(filename, begin, (int)(end - begin));
}

CStrs FindAllFilesUp(const char *fullDirPath, const CStrs *filenamesToSearch) {
    CStrs paths = {0};
    char dirPath[MAX_PATH];
    strcpy(dirPath, fullDirPath);

    do {
        // printf("Exploring dir: '%s'\n", dirPath);
        CStrs filesInDir = GetFiles(dirPath);

        for (size_t idxDir = 0; idxDir < filesInDir.count; ++idxDir) {
            const char *fullPath = filesInDir.items[idxDir];
            // printf("  In that dir: '%s'\n", fullPath);
            if (!IsFile(fullPath)) continue;
            // printf("    is file:\n");

            for (size_t idxSearch = 0; idxSearch < filenamesToSearch->count; ++idxSearch) {
                const char *filename = GetFilename(fullPath);
                // printf("    pure filename: '%s'\n", filename);
                const char *filenameSearch = filenamesToSearch->items[idxSearch];
                if (!strcmp(filename, filenameSearch)) {
                    // printf("    match!\n");
                    char *newPath = malloc(MAX_PATH + 1);
                    strcpy(newPath, fullPath);
                    // printf("    add to result: '%s'\n", newPath);
                    DArrayAppend(&paths, newPath);
                    break;
                }
                // printf("    miss!\n");
            }
        }

        strcpy(dirPath, CutPath(dirPath));

        CStrsFree(&filesInDir);
    } while (*dirPath);
    return paths; 
}




