#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int usage(void) {
    fprintf(stderr,
            "usage: aksa tokens <file.aksa> [--locale id]\n"
            "       aksa run <file.aksa> [--locale id]\n");
    return 2;
}

int main(int argc, char **argv) {
    if (argc < 3) return usage();
    const char *cmd = argv[1], *file = argv[2], *locale = "id";
    for (int i = 3; i < argc - 1; i++)
        if (strcmp(argv[i], "--locale") == 0) locale = argv[i + 1];

    if (strcmp(cmd, "run") == 0) {
        fprintf(stderr, "aksa run: not yet — parser + VM land in Phase 1\n");
        return 2;
    }
    if (strcmp(cmd, "tokens") != 0) return usage();

    char path[512];
    snprintf(path, sizeof path, "locales/%s.json", locale);
    char *json = aksa_read_file(path);
    if (!json) {
        fprintf(stderr, "aksa: cannot read %s (run from the repo root)\n", path);
        return 1;
    }
    AksaLocale loc;
    char lerr[128];
    if (aksa_locale_load(&loc, json, lerr, sizeof lerr)) {
        fprintf(stderr, "aksa: %s: %s\n", path, lerr);
        free(json);
        return 1;
    }
    free(json);

    char *src = aksa_read_file(file);
    if (!src) {
        fprintf(stderr, "aksa: cannot read %s\n", file);
        return 1;
    }
    int nerr = 0;
    char *out = aksa_dump_tokens(src, &loc, &nerr);
    fputs(out, stdout);
    free(out);
    free(src);
    return nerr ? 1 : 0;
}
