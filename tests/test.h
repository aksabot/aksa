#ifndef AKSA_TEST_H
#define AKSA_TEST_H
#include <stdio.h>
#include <string.h>

static int t_run = 0, t_fail = 0;

#define CHECK(cond) do { \
    t_run++; \
    if (!(cond)) { t_fail++; printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); } \
} while (0)

#define CHECK_STR(a, b) do { \
    t_run++; \
    const char *_a = (a), *_b = (b); \
    if (!_a || strcmp(_a, _b) != 0) { \
        t_fail++; \
        printf("FAIL %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, _a ? _a : "(null)", _b); \
    } \
} while (0)

#define TEST_SUMMARY() \
    (printf("%s: %d checks, %d failed\n", __FILE__, t_run, t_fail), t_fail ? 1 : 0)

#endif
