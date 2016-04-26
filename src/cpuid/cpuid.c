#include "common.h"

#define _EFLAGS_ID_MASK 0x00200000

static bool naked _CpuidIsSupported()
{
    asm {
        pushfd
        pushfd
        xor dword ptr [sp], _EFLAGS_ID_MASK
        popfd
        pushfd
        pop eax
        xor eax, [sp]
        popfd
        and eax, _EFLAGS_ID_MASK
        jz returnResult
        mov ax, TRUE
    returnResult:
        ret
    }
}

extern void main()
{
    if ( _CpuidIsSupported() ) {
        printf( "CPUID is supported!!!" );
    } else {
        printf( "CPUID is not supported :(" );
    }
}
