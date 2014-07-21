#pragma once

#include "process.h"

extern void SwitchContextToKernel();
extern void SwitchContextToProcess();
extern void InitProcessContext( Process *pProcess );
extern void InitKernelContext();
