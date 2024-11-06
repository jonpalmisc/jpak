#include "jpak.h"

#include "io.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    MODE_INVALID,
    MODE_CREATE,
    MODE_EXTRACT,
    MODE_LIST,
} Mode;

static Mode mode_from_str(char const *str)
{
    if (strncmp(str, "c", 1) == 0)
        return MODE_CREATE;
    if (strncmp(str, "x", 1) == 0)
        return MODE_EXTRACT;
    if (strncmp(str, "l", 1) == 0)
        return MODE_LIST;

    return MODE_INVALID;
}

typedef enum {
    USAGE_SHORT,
    USAGE_FULL
} UsageStyle;

static void show_usage(UsageStyle style)
{
    fprintf(stderr, "Usage: jpak {c|x|l} <archive> [<files>...]\n");
    if (style != USAGE_FULL)
        return;
}

#define ITEM_OFFSET(N) (sizeof(Jpak) + (sizeof(JpakEntry) * (N)))

int archive_create(char const *archive_path, char const **file_list, int num_files)
{
    for (int i = 0; i < num_files; ++i) {
        if (!file_exists(file_list[i])) {
            fprintf(stderr, "Error: Input file does not exist: %s\n", file_list[i]);
            return EXIT_FAILURE;
        }
    }

    FILE *archive_file = fopen(archive_path, "wb");
    if (!archive_file) {
        fprintf(stderr, "Error: Failed to open output file.");
        return EXIT_FAILURE;
    }

    Jpak jpak;
    jpak_init(&jpak, num_files);
    fwrite(&jpak, sizeof(jpak), 1, archive_file);

    // Pre-write/allocate space for the item headers.
    for (int i = 0; i < num_files; ++i) {
        JpakEntry item;
        jpak_entry_init(&item, 0, 0, file_list[i]);
        fwrite(&item, sizeof(item), 1, archive_file);
    }

    unsigned running_offset = sizeof(Jpak) + (sizeof(JpakEntry) * num_files);
    for (int i = 0; i < num_files; ++i) {
        uint8_t *file_data = NULL;
        int file_size = 0;
        if (!file_read_alloc(file_list[i], &file_data, &file_size)) {
            fprintf(stderr, "Error: Failed to read file '%s', aborting...", file_list[i]);
            fclose(archive_file);
            return EXIT_FAILURE;
        }

        fwrite(file_data, 1, file_size, archive_file);

        JpakEntry item;
        jpak_entry_init(&item, running_offset, file_size, file_list[i]);
        fseek(archive_file, ITEM_OFFSET(i), SEEK_SET);
        fwrite(&item, sizeof(item), 1, archive_file);

        running_offset += file_size;
        fseek(archive_file, running_offset, SEEK_SET);
    }

    fclose(archive_file);
    return EXIT_SUCCESS;
}

static Jpak const *archive_read_alloc(char const *path)
{
    uint8_t *raw = NULL;
    int raw_size = 0;
    if (!file_read_alloc(path, &raw, &raw_size))
        return NULL;

    return (Jpak const *)raw;
}

int archive_extract(char const *archive_path)
{
    Jpak const *archive = archive_read_alloc(archive_path);
    if (!archive) {
        fprintf(stderr, "Error: Failed to read archive!");
        return EXIT_FAILURE;
    }
    if (!jpak_is_valid(archive)) {
        fprintf(stderr, "Error: Archive is invalid!");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < archive->num_entries; ++i) {
        JpakEntry const *item = jpak_entry(archive, i);
        assert(item);

        if (!file_write((char const *)item->name, jpak_data(archive, item), item->size)) {
            fprintf(stderr, "Error: Failed to write file!\n");
            return EXIT_FAILURE;
        }
    }

    free((void *)archive);
    return EXIT_SUCCESS;
}

int archive_list(char const *archive_path)
{
    Jpak const *archive = archive_read_alloc(archive_path);
    if (!archive || !jpak_is_valid(archive)) {
        fprintf(stderr, "Error: Failed to read archive or archive is invalid!");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < archive->num_entries; ++i) {
        JpakEntry const *item = jpak_entry(archive, i);
        assert(item);

        fprintf(stderr, "%d: '%s', offset=%d, size=%d\n", i, item->name, item->offset, item->size);
    }

    return EXIT_SUCCESS;
}

int main(int argc, char const **argv)
{
    if (argc < 3) {
        show_usage(USAGE_FULL);
        return EXIT_FAILURE;
    }

    Mode mode = mode_from_str(argv[1]);
    if (mode == MODE_INVALID) {
        fprintf(stderr, "Error: Invalid mode!\n");
        show_usage(USAGE_SHORT);
        return EXIT_FAILURE;
    }

    char const *archive_path = argv[2];
    if (mode == MODE_CREATE) {
        if (argc == 3) {
            fprintf(stderr, "Error: No files provided!\n");
            show_usage(USAGE_SHORT);
            return EXIT_FAILURE;
        }

        char const **file_list = &argv[3];
        return archive_create(archive_path, file_list, argc - 3);
    }

    if (mode == MODE_EXTRACT)
        return archive_extract(archive_path);

    return archive_list(archive_path);
}
