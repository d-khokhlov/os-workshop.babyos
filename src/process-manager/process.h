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

#define MAX_PROCESS_PARAMETERS_COUNT 1
typedef int ProcessParameters[ MAX_PROCESS_PARAMETERS_COUNT ];

typedef struct Process
{
    ProcessState state;
    Registers registers;
    ProcessParameters parameters;
} Process;
