#ifndef CCL_DSTRING_H
#define CCL_DSTRING_H

#include <stdio.h>
#include <stdbool.h>

#include "darray.h"

INSTANTIATE_DARRAY(DString, char)

void DStringResize(DString *string, size_t new_capacity);
void DStringAppend(DString *string, char item);
void DStringExtend(DString *string, const char *c_str);
void DStringExtendBuffer(DString *string, const char *buffer, size_t count);
void DStringConcat(DString *string, const DString *other);

void DStringFree(DString *string);
void DStringClear(DString *string);

bool DStringReadFile(DString *string, const char *file_path);

#endif // CCL_DSTRING_H