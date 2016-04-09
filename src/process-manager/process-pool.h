#pragma once

#include "common.h"
#include "process.h"

extern void ProcessPool_Init();
extern void ProcessPool_ChooseProcessAndActivate();
extern Process *ProcessPool_GetActiveProcess();
extern Process *ProcessPool_CreateProcess();
extern bool ProcessPool_DestroyActiveProcess();
