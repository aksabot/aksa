#ifndef AKSA_RT_H
#define AKSA_RT_H

/* Runtime for emitted C programs. A tagged value mirroring the VM's Val
   (core/vm.c) so compiled programs match the interpreter exactly. Value
   semantics live here once; the differential tests (tests/diff.sh) guard
   against drift from the VM. */

typedef enum { AK_NIL, AK_NUM, AK_BOOL, AK_STR } AkTag;

typedef struct {
    AkTag t;
    double num;      /* AK_NUM value; AK_BOOL 0/1 */
    const char *str; /* AK_STR text */
} Ak;

Ak ak_nil(void);
Ak ak_num(double n);
Ak ak_bool(int b);
Ak ak_str(const char *s);

double ak_asnum(Ak v);
int ak_asbool(Ak v);

/* line = kid source line, for the E104/E101 runtime error position */
Ak ak_add(Ak a, Ak b, int line);
Ak ak_sub(Ak a, Ak b, int line);
Ak ak_mul(Ak a, Ak b, int line);
Ak ak_div(Ak a, Ak b, int line);
Ak ak_mod(Ak a, Ak b, int line);
Ak ak_lt(Ak a, Ak b, int line);
Ak ak_gt(Ak a, Ak b, int line);
Ak ak_le(Ak a, Ak b, int line);
Ak ak_ge(Ak a, Ak b, int line);
Ak ak_eq(Ak a, Ak b);
Ak ak_neq(Ak a, Ak b);
Ak ak_not(Ak a, int line);
Ak ak_neg(Ak a, int line);

Ak ak_print(int n, ...);
Ak ak_ask(Ak prompt);

/* hardware builtins (routed through the HAL, hal.h) */
Ak ak_pin_on(Ak pin);
Ak ak_pin_off(Ak pin);
Ak ak_pin_read(Ak pin);
Ak ak_pin_read_analog(Ak pin);
Ak ak_wait(Ak ms);
Ak ak_unsupported(const char *name, int line); /* turtle etc.: E106 */

/* load locales/<name>.json for runtime error messages + boolean words */
void aksa_rt_init(const char *locale);
/* print "! <id> <line>:0 <localized msg>" to stderr and exit(1) */
void aksa_rt_error(const char *id, int line, const char *arg);

#endif
