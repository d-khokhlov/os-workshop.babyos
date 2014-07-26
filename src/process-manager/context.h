#pragma once

#include "processes.h"

extern void SwitchContextToKernel();
extern void SwitchContextToProcess();
extern void InitProcessContext( Process *pProcess );
extern void InitKernelContext();
