#include <string.h>
#include <stdlib.h>

#include "darray.h"
#include "dstring.h"

void DStringResize(DString *string, size_t new_capacity) {
    if (!string) {
        return;
    }

    DArrayResize_Impl(string, sizeof(char) /*1*/, new_capacity);
    DStringAppend(string, '\0');
}

void DStringAppend(DString *string, char item) {
    if (!string) {
        return;
    }

    // do nothing if there is null terminator already
    if (item == '\0' && string->count < string->capacity && string->items[string->count + 1] == '\0') {
        return;
    }

	while (string->capacity < string->count + 2) {
        DStringResize(string, string->capacity ? string->capacity * 1.5f + 1 : DARRAY_INIT_CAPACITY);
    }

    if (item != '\0') {
        string->items[string->count++] = item;
    }
    string->items[string->count] = '\0';
}

void DStringExtend(DString *string, const char *str) {
    if (!string) {
        return;
    }

    DStringExtendBuffer(string, str, strlen(str));
}

void DStringExtendBuffer(DString *string, const char *buffer, size_t count) {
    if (!string) {
        return;
    }

	while (string->capacity < string->count + count) {
        DStringResize(string, string->capacity ? string->capacity * 1.5f + 1 : DARRAY_INIT_CAPACITY);
    }

    for (size_t i = 0; i < count; ++i) {
        string->items[string->count++] = buffer[i];
    }

    string->items[string->count] = '\0';
}

void DStringConcat(DString *string, const DString *other) {
    if (!string) {
        return;
    }

    DStringExtendBuffer(string, other->items, other->count);
}

void DStringFree(DString *string) {
    free(string->items);
}

void DStringClear(DString *string) {
    if (!string) {
        return;
    }

    free(string->items);
    string->items = NULL;
    string->count = 0;
    string->capacity = 0;
}

bool DStringReadFile(DString *string, const char *file_path) {
    if (!string) {
        return false;
    }

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        return false;
    }

    if (fseek(file, 0, SEEK_END)) {
        return false;
    }

    long file_size = ftell(file);
    if (file_size < 0) {
        return false;
    }

    if (fseek(file, 0, SEEK_SET)) {
        return false;
    }

    DStringResize(string, string->capacity + file_size);
    fread(string->items + string->count, 1, file_size, file);
    string->count += file_size;
    DStringAppend(string, '\0');

    if (ferror(file)) {
        return false;
    }

    return true;
}