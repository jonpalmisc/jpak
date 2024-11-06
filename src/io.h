#pragma once

#include <stdint.h>

/// Does a file exist?
int file_exists(char const *path);

/// Read an entire binary file into memory.
int file_read_alloc(char const *path, uint8_t **out, int32_t *size);

/// Write data to a file.
int file_write(char const *path, uint8_t const *data, int32_t size);
