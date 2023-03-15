#include "stack-util.h"

__attribute__((no_instrument_function)) void __cyg_profile_func_enter(void* this, void* call)
{
    
}
int MINSP;
__attribute__((no_instrument_function)) void __cyg_profile_func_exit(void* this, void* call)
{
    //uint32_t cur_sp = GETSP() + 8;
   
    uint32_t cur_sp = GETSP() + 8;
   // if(cur_sp<0x20000000)
    //printf("\ncursp= %lx\n",cur_sp);  
     if (cur_sp < MINSP) {
        MINSP = cur_sp;
    }
    // use debugger to output current stack pointer
    // for example semihosting on ARM
    //   __DEBUGGER_TRANSFER_STACK_POINTER(cur_stack_usage);
    // or you could store the max somewhere
    // then just run the program
   
}