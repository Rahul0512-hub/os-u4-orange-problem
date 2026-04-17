#include "commit.h"
#include <stdio.h>

// Dummy implementation for commit_create
int commit_create(const char *message, ObjectID *commit_id_out) {
    (void)message;
    (void)commit_id_out;

    printf("Commit created\n");
    return 0;
}

// Dummy implementation for commit_walk
int commit_walk(commit_walk_fn callback, void *ctx) {
    (void)callback;
    (void)ctx;

    printf("Commit walk executed\n");
    return 0;
}
