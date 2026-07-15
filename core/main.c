#include "checker.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef AKSA_DEFAULT_LOCALE
#define AKSA_DEFAULT_LOCALE "en"
#endif

/* Headless board: pin writes echo to stdout, reads are always 0, waiting
   is skipped. Keeps hardware programs runnable and testable on the CLI. */
static int cli_host(const char *canon, double num, const char *str,
                    double *result, void *user) {
    (void)str;
    (void)user;
    if (strcmp(canon, "pin_on") == 0) printf("[pin %g ON]\n", num);
    else if (strcmp(canon, "pin_off") == 0) printf("[pin %g OFF]\n", num);
    else if (strcmp(canon, "pin_read") == 0 ||
             strcmp(canon, "pin_read_analog") == 0) *result = 0;
    else if (strcmp(canon, "wait") == 0) { /* no delay */ }
    else return 0; /* turtle builtins still need the browser */
    return 1;
}

static int usage(void) {
    fprintf(stderr,
            "usage: aksa tokens <file.aksa> [--locale en]\n"
            "       aksa ast <file.aksa> [--locale en]\n"
            "       aksa check <file.aksa> [--locale en]\n"
            "       aksa run <file.aksa> [--locale en]\n");
    return 2;
}

int main(int argc, char **argv) {
    if (argc < 3) return usage();
    const char *cmd = argv[1], *file = argv[2], *locale = AKSA_DEFAULT_LOCALE;
    for (int i = 3; i < argc - 1; i++)
        if (strcmp(argv[i], "--locale") == 0) locale = argv[i + 1];

    if (strcmp(cmd, "tokens") != 0 && strcmp(cmd, "ast") != 0 &&
        strcmp(cmd, "check") != 0 && strcmp(cmd, "run") != 0)
        return usage();

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
    if (strcmp(cmd, "run") == 0 || strcmp(cmd, "check") == 0) {
        AksaErrors errs = {0};
        int rc;
        if (strcmp(cmd, "run") == 0) {
            AksaHost host = {.host = cli_host};
            rc = aksa_run(src, &loc, &errs, &host);
        } else {
            AksaNode *prog = aksa_parse(src, &loc, &errs);
            if (errs.count == 0) aksa_check(prog, &loc, &errs);
            aksa_ast_free(prog);
            rc = errs.count > 0;
        }
        size_t cap = 64, len = 0;
        char *msgs = malloc(cap);
        msgs[0] = 0;
        aksa_errors_dump(&loc, &errs, &msgs, &len, &cap);
        fputs(msgs, stderr);
        free(msgs);
        free(src);
        return rc ? 1 : 0;
    }
    char *out = strcmp(cmd, "ast") == 0 ? aksa_dump_ast(src, &loc, &nerr)
                                        : aksa_dump_tokens(src, &loc, &nerr);
    fputs(out, stdout);
    free(out);
    free(src);
    return nerr ? 1 : 0;
}
