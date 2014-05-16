#pragma once

#include "registers.h"

typedef enum {
    PROCESS_STATE_NULL,
    PROCESS_STATE_READY,
    PROCESS_STATE_ACTIVE,
    PROCESS_STATE_BLOCKED,
} ProcessState;

typedef int ProcessId;
#define PROCESS_ID_NONE -1

typedef struct {
    ProcessState state;
    Registers registers;
    ProcessId parentId;
} Process;
