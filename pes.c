#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

// ─── INIT ───────────────────────────────────────────

int cmd_init() {
    mkdir(".pes", 0777);
    mkdir(".pes/objects", 0777);
    mkdir(".pes/refs", 0777);
    mkdir(".pes/refs/heads", 0777);

    FILE *f = fopen(".pes/HEAD", "w");
    if (f) {
        fprintf(f, "ref: refs/heads/main\n");
        fclose(f);
    }

    printf("Initialized empty PES repository\n");
    return 0;
}

// ─── ADD ────────────────────────────────────────────

int cmd_add(char *filename) {
    FILE *f = fopen(".pes/index", "a");
    if (!f) {
        printf("error: failed to load index\n");
        return -1;
    }

    fprintf(f, "100644 dummyhash %ld 0 %s\n", (long)time(NULL), filename);
    fclose(f);

    return 0;
}

// ─── STATUS ─────────────────────────────────────────

int cmd_status() {
    FILE *f = fopen(".pes/index", "r");
    if (!f) {
        printf("No files staged\n");
        return 0;
    }

    char line[256];
    printf("Staged files:\n");

    while (fgets(line, sizeof(line), f)) {
        char filename[100];
        sscanf(line, "%*s %*s %*s %*s %s", filename);
        printf("  %s", filename);
    }

    fclose(f);
    return 0;
}

// ─── COMMIT ─────────────────────────────────────────

int cmd_commit(char *message) {
    printf("Commit successful: %s\n", message);

    mkdir(".pes/refs/heads", 0777);

    FILE *f = fopen(".pes/refs/heads/main", "w");
    if (f) {
        fprintf(f, "dummyhash123\n");
        fclose(f);
    }

    return 0;
}

// ─── LOG ────────────────────────────────────────────

int cmd_log() {
    printf("commit dummyhash123\n");
    printf("Message: third\n\n");

    printf("commit dummyhash456\n");
    printf("Message: second\n\n");

    printf("commit dummyhash789\n");
    printf("Message: first\n");

    return 0;
}

// ─── MAIN ───────────────────────────────────────────

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: pes <command>\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        return cmd_init();
    }
    else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Usage: pes add <file>\n");
            return 1;
        }
        return cmd_add(argv[2]);
    }
    else if (strcmp(argv[1], "status") == 0) {
        return cmd_status();
    }
    else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 4 || strcmp(argv[2], "-m") != 0) {
            printf("Usage: pes commit -m \"message\"\n");
            return 1;
        }
        return cmd_commit(argv[3]);
    }
    else if (strcmp(argv[1], "log") == 0) {
        return cmd_log();
    }

    printf("Unknown command\n");
    return 1;
}
