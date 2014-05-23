#include "process.h"
#include "process-manager.h"
#include "common.h"

void _cdecl HandleClockInterrupt( SegmentRegister *ss, CommonRegister *sp )
{
    Registers *pRegisters;

    pRegisters = GetActiveProcessRegisters();
    if ( pRegisters != NULL ) {
        pRegisters->ss = *ss;
        pRegisters->sp = *sp;
    }

    ActivateProcess( ChooseProcessToActivate() );

    pRegisters = GetActiveProcessRegisters();
    if ( pRegisters != NULL ) {
        *ss = pRegisters->ss;
        *sp = pRegisters->sp;
    }
}
