#include "index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Forward declaration (avoids warning)
int object_write(const char *path, ObjectID *oid);

// ─── PROVIDED ─────────────────────────────────────────

IndexEntry* index_find(Index *index, const char *path) {
    for (int i = 0; i < index->count; i++) {
        if (strcmp(index->entries[i].path, path) == 0)
            return &index->entries[i];
    }
    return NULL;
}

// ─── LOAD INDEX ───────────────────────────────────────

int index_load(Index *index) {
    FILE *fp = fopen(".pes/index", "r");

    index->count = 0;

    if (!fp) return 0;

    char path[256], hash_hex[65];
    unsigned int mode;
    long mtime;
    unsigned int size;

    while (fscanf(fp, "%o %64s %ld %u %255s",
                  &mode, hash_hex, &mtime, &size, path) == 5) {

        IndexEntry *e = &index->entries[index->count++];

        e->mode = mode;
        e->mtime_sec = mtime;
        e->size = size;
        strcpy(e->path, path);

        hex_to_hash(hash_hex, &e->hash);
    }

    fclose(fp);
    return 0;
}

// ─── SAVE INDEX ───────────────────────────────────────

int index_save(const Index *index) {
    FILE *fp = fopen(".pes/index", "w");
    if (!fp) {
        perror("index save failed");
        return -1;
    }

    char hash_hex[65];

    for (int i = 0; i < index->count; i++) {
        hash_to_hex(&index->entries[i].hash, hash_hex);

        fprintf(fp, "%o %s %ld %u %s\n",
                index->entries[i].mode,
                hash_hex,
                index->entries[i].mtime_sec,
                index->entries[i].size,
                index->entries[i].path);
    }

    fclose(fp);
    return 0;
}

// ─── ADD FILE ─────────────────────────────────────────

int index_add(Index *index, const char *path) {
    struct stat st;

    if (stat(path, &st) != 0) {
        perror("file not found");
        return -1;
    }

    ObjectID oid;

    if (object_write(path, &oid) != 0) {
        return -1;
    }

    IndexEntry *e = index_find(index, path);

    if (!e) {
        e = &index->entries[index->count++];
    }

    e->mode = st.st_mode;
    e->mtime_sec = st.st_mtime;
    e->size = st.st_size;
    strcpy(e->path, path);

    e->hash = oid;

    return index_save(index);
}

// ─── STATUS (FIXES YOUR ERROR) ───────────────────────

int index_status(const Index *index) {
    printf("Staged files:\n");

    if (index->count == 0) {
        printf("  (none)\n");
        return 0;
    }

    for (int i = 0; i < index->count; i++) {
        printf("  %s\n", index->entries[i].path);
    }

    return 0;
}
