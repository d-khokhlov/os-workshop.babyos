#include "registers.h"
#include "process.h"
#include "mocks.h"

CommonRegister _cdecl SaveNewContext(
    SegmentRegister cs, CommonRegister ip,
    SegmentRegister ss, CommonRegister sp,
    SegmentRegister ds, SegmentRegister es,
    int parameter );

void InitProcessContext( Process *pProcess )
{
    pProcess->registers.sp = SaveNewContext(
        pProcess->registers.cs, pProcess->registers.ip,
        pProcess->registers.ss, pProcess->registers.sp,
        pProcess->registers.ds, pProcess->registers.es,
        pProcess->parameters[ 0 ] );
}
