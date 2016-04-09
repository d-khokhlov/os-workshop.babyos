#pragma once

#include <common.h>
#include "process.h"
#include "architecture.h"

typedef packed struct ProcessContext
{
    SegmentRegister es, ds;
    CommonRegister di, si, bp, sp, bx, dx, cx, ax;
    InstructionPointer ip;
    SegmentRegister cs;
    FlagsRegister flags;
} ProcessContext;

extern void SwitchContextToKernel();
extern void SwitchContextToProcess();
extern void InitProcessContext( Process *pProcess );
extern void InitKernelContext();
