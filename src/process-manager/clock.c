#include "process.h"
#include "process-manager.h"
#include "common.h"

void public HandleClockInterrupt( SegmentRegister *pSs, CommonRegister *pSp )
{
    Registers *pRegisters;

    pRegisters = GetActiveProcessRegisters();
    if ( pRegisters != NULL ) {
        pRegisters->ss = *pSs;
        pRegisters->sp = *pSp;
    }

    ActivateProcess( ChooseProcessToActivate() );

    pRegisters = GetActiveProcessRegisters();
    if ( pRegisters != NULL ) {
        *pSs = pRegisters->ss;
        *pSp = pRegisters->sp;
    }
}
