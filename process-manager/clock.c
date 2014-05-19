#include "process.h"
#include "process-manager.h"

void _cdecl HandleClockInterrupt( SegmentRegister *ss, CommonRegister *sp ) {

    Registers *pRegisters;

    pRegisters = GetActiveProcessRegisters();
    pRegisters->ss = *ss;
    pRegisters->sp = *sp;

    ActivateProcess( ChooseProcessToActivate() );

    pRegisters = GetActiveProcessRegisters();
    *ss = pRegisters->ss;
    *sp = pRegisters->sp;
}
