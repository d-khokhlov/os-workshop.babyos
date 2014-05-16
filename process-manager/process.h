#pragma once

#include "registers.h"

typedef enum {
    ProcessState_Null,
    ProcessState_Ready,
    ProcessState_Active,
    ProcessState_Blocked,
} ProcessState;

typedef int ProcessId;
#define ProcessId_None -1

typedef struct {
    ProcessState state;
    Registers registers;
    ProcessId parentId;
} Process;
