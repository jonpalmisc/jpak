//
//  jpak.c
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

#include "jpak.h"

#include <string.h>

void jpak_entry_init(JpakEntry *entry, uint32_t offset, int32_t size, char const *name)
{
    entry->offset = offset;
    entry->size = size;

    strncpy((char *)&entry->name, name, sizeof(entry->name) - 1);
}

void jpak_init(Jpak *jpak, int32_t num_entries)
{
    jpak->magic = JPAK_HEADER_MAGIC;
    jpak->version = JPAK_VERSION;
    jpak->num_entries = num_entries;
    jpak->reserved = -1;
}

uint8_t jpak_is_valid(Jpak const *jpak)
{
    return jpak->magic == JPAK_HEADER_MAGIC && jpak->num_entries > 0;
}

int32_t jpak_index(Jpak const *jpak, char const *name)
{
    for (int i = 0; i < jpak->num_entries; ++i) {
        char const *entry_name = (char const *)jpak->_entries[i].name;
        if (strncmp(entry_name, name, JPAK_ENTRY_NAME_MAX) == 0)
            return i;
    }

    return -1;
}

JpakEntry const *jpak_entry(Jpak const *jpak, int32_t index)
{
    if (index < 0 || index >= jpak->num_entries)
        return NULL;

    return &jpak->_entries[index];
}

uint8_t const *jpak_data(Jpak const *jpak, JpakEntry const *entry)
{
    return (uint8_t const *)jpak + entry->offset;
}
