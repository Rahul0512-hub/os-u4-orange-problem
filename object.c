#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

// ─── HEX CONVERSION ─────────────────────────────────

void hash_to_hex(const ObjectID *id, char *hex_out) {
    for (int i = 0; i < HASH_SIZE; i++) {
        sprintf(hex_out + (i * 2), "%02x", id->hash[i]);
    }
    hex_out[HASH_SIZE * 2] = '\0';
}

int hex_to_hash(const char *hex, ObjectID *id_out) {
    for (int i = 0; i < HASH_SIZE; i++) {
        if (sscanf(hex + (i * 2), "%2hhx", &id_out->hash[i]) != 1)
            return -1;
    }
    return 0;
}

// ─── OBJECT PATH ─────────────────────────────────────

void object_path(const ObjectID *id, char *path_out, size_t path_size) {
    char hex[HASH_HEX_SIZE + 1];
    hash_to_hex(id, hex);
    snprintf(path_out, path_size, "%s/%s", OBJECTS_DIR, hex);
}

// ─── CHECK EXISTENCE ─────────────────────────────────

int object_exists(const ObjectID *id) {
    char path[512];
    object_path(id, path, sizeof(path));
    FILE *f = fopen(path, "rb");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

// ─── COMPUTE HASH (SHA-256) ─────────────────────────

static void compute_hash(ObjectType type, const void *data, size_t len, uint8_t *out) {
    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    // stable type (1 byte)
    uint8_t t = (uint8_t)type;
    SHA256_Update(&ctx, &t, 1);

    SHA256_Update(&ctx, data, len);
    SHA256_Final(out, &ctx);
}

// ─── WRITE OBJECT ───────────────────────────────────

int object_write(ObjectType type, const void *data, size_t len, ObjectID *id_out) {
    uint8_t hash[HASH_SIZE];

    compute_hash(type, data, len, hash);

    memset(id_out, 0, sizeof(ObjectID));  // ensure clean
    memcpy(id_out->hash, hash, HASH_SIZE);

    char path[512];
    object_path(id_out, path, sizeof(path));

    // Deduplication
    if (object_exists(id_out)) {
        return 0;
    }

    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("object write failed");
        return -1;
    }

    fwrite(&type, sizeof(type), 1, f);
    fwrite(&len, sizeof(len), 1, f);
    fwrite(data, 1, len, f);

    fclose(f);
    return 0;
}

// ─── READ OBJECT ────────────────────────────────────

int object_read(const ObjectID *id, ObjectType *type_out, void **data_out, size_t *len_out) {
    char path[512];
    object_path(id, path, sizeof(path));

    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    ObjectType type;
    size_t len;

    if (fread(&type, sizeof(type), 1, f) != 1) {
        fclose(f);
        return -1;
    }

    if (fread(&len, sizeof(len), 1, f) != 1) {
        fclose(f);
        return -1;
    }

    void *data = malloc(len);
    if (!data) {
        fclose(f);
        return -1;
    }

    if (fread(data, 1, len, f) != len) {
        free(data);
        fclose(f);
        return -1;
    }

    fclose(f);

    // Integrity check
    uint8_t hash[HASH_SIZE];
    compute_hash(type, data, len, hash);

    if (memcmp(hash, id->hash, HASH_SIZE) != 0) {
        free(data);
        return -1;
    }

    *type_out = type;
    *data_out = data;
    *len_out = len;

    return 0;
}
