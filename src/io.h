#pragma once

#include <nds.h>

typedef struct
{
    int size;
    u8* data;
} Bytes;

// Find out a file's size, allocate memory and read the whole thing in
// The caller is responsible for freeing Bytes.data.
Bytes slurp(const char *path);

void freeBytes(Bytes* bytes);
