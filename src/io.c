#include "io.h"

#include <stdio.h>
#include <stdlib.h>

int file_exists(char const *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
        return 0;

    fclose(file);
    return 1;
}

int file_read_alloc(char const *path, uint8_t **out, int32_t *size)
{
    FILE *file = fopen(path, "rb");
    if (!file)
        return 0;

    fseek(file, 0, SEEK_END);
    *size = (int32_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    *out = malloc(*size);
    *size = (int32_t)fread(*out, 1, *size, file);
    fclose(file);
    return 1;
}

int file_write(char const *path, uint8_t const *data, int32_t size)
{
    FILE *file = fopen(path, "wb");
    if (!file)
        return 0;

    fwrite(data, 1, size, file);
    fclose(file);
    return 1;
}
