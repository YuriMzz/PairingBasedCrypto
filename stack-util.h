#ifndef INSTUMENT_FUNCTION_H
#define INSTUMENT_FUNCTION_H

#include <stdint.h>
#include <stdio.h>

extern int GETSP(void);
extern int MINSP;

#define STACK_USAGE(FUNCTION, S) \
    MINSP = GETSP();             \
    FUNCTION;\
    (S) = GETSP() - MINSP;

void __cyg_profile_func_enter(void* this, void* call) __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void* this, void* call) __attribute__((no_instrument_function));

#endif /* INSTUMENT_FUNCTION_H */