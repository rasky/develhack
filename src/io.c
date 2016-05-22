#include "io.h"

#include <stdio.h>

#include "debug.h"

Bytes slurp(const char* path)
{
    FILE* fp = fopen(path, "rb");
    if (fseek(fp, 0, SEEK_END) != 0) {
        debugf("Error seeking to end of in %s\n", path);
    }

    long size = ftell(fp);
    if (size == -1L) {
        debugf("Error getting size of %s\n", path);
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        debugf("Error seeking back to 0 in %s\n", path);
    }

    Bytes blob;
    blob.size = size;
    blob.data = (u8*)malloc(size);

    if (!blob.data) {
        debugf("Error allocating %i bytes to read %s with\n", size, path);
    }

    size_t read = fread(blob.data, sizeof(u8), size, fp);
    if (read != size) {
        debugf("slurp: Expected %i bytes, got %i from %s\n", size, read, path);
    }

    debugf("Read %i from %s\n", read, path);

    return blob;
}
