//
//  jpak.h
//  https://github.com/jonpalmisc/jpak
//
//  This is free and unencumbered software released into the public domain.
//
//  Anyone is free to copy, modify, publish, use, compile, sell, or distribute
//  this software, either in source code form or as a compiled binary, for any
//  purpose, commercial or non-commercial, and by any means.
//
//  In jurisdictions that recognize copyright laws, the author or authors of
//  this software dedicate any and all copyright interest in the software to
//  the public domain. We make this dedication for the benefit of the public at
//  large and to the detriment of our heirs and successors. We intend this
//  dedication to be an overt act of relinquishment in perpetuity of all
//  present and future rights to this software under copyright law.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
//  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//  For more information, please refer to <https://unlicense.org>
//

#pragma once

#include <stdint.h>

#define JPAK_HEADER_MAGIC 'KAPJ'
#define JPAK_VERSION 1
#define JPAK_ENTRY_NAME_MAX 24

#ifdef __cplusplus
extern "C" {
#endif

/// Archive entry descriptor.
///
/// Tags a span of content within the archive with a name.
typedef struct {
    uint32_t offset; ///< Offset to content, measured from start of the archive.
    int32_t size;    ///< Size of the content in bytes.

    int8_t name[JPAK_ENTRY_NAME_MAX]; ///< Content name.
} JpakEntry;

/// Initialize an entry.
void jpak_entry_init(JpakEntry *entry, uint32_t offset, int32_t size, char const *name);

/// Archive header and handle.
///
/// An array of entries and the archive content is assumed to immediately
/// follow this structure in memory.
typedef struct {
    uint32_t magic;
    uint32_t version;
    int32_t num_entries;
    uint32_t reserved;

    JpakEntry _entries[0];
} Jpak;

/// Initialize an archive.
///
/// Only needed when creating an archive in memory.
void jpak_init(Jpak *jpak, int32_t num_entries);

/// Tells if an archive is valid.
///
/// Should always be called after reading an archive into memory, etc.
uint8_t jpak_is_valid(Jpak const *jpak);

/// Find an entry with the given name and get its index.
///
/// Returns -1 on failure.
int32_t jpak_index(Jpak const *jpak, char const *name);

/// Get a pointer to an entry descriptor by its index.
///
/// Returns NULL if the index was invalid.
JpakEntry const *jpak_entry(Jpak const *jpak, int32_t index);

/// Get a pointer to an entry's data.
///
/// Does not validate the entry; should only be used with entries returned by
/// other functions.
uint8_t const *jpak_data(Jpak const *jpak, JpakEntry const *entry);

#ifdef __cplusplus
}
#endif
