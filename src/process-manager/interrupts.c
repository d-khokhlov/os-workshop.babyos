#include "interrupts.h"
#include "common.h"
#include "mocks.h"

extern void asmcall SetInterruptHandler( unsigned short number, void far *pHandler )
{
    void far * far *ppVector = MakeFp( 0, number * 4 );
    asm {
        pushf
        cli
    }
    *ppVector = pHandler;
    asm {
        popf
    }
}

extern void far * asmcall GetInterruptHandler( unsigned short number )
{
    void far * far *ppVector = MakeFp( 0, number * 4 );
    return *ppVector;
}
