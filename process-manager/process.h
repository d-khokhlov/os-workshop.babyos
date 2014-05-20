#pragma once

#include "registers.h"

typedef enum ProcessState
{
    ProcessState_Null,
    ProcessState_Ready,
    ProcessState_Active,
    ProcessState_Blocked,
} ProcessState;

typedef int ProcessId;
#define ProcessId_None -1

typedef struct Process
{
    ProcessState state;
    Registers registers;
    ProcessId parentId;
} Process;
