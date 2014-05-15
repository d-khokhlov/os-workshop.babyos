#pragma once

#include "registers.h"

typedef enum {

    PROCESS_STATE_VOID,
    PROCESS_STATE_READY,
    PROCESS_STATE_ACTIVE,
    PROCESS_STATE_BLOCKED,

} ProcessState;

typedef struct {

    ProcessState state;
    Registers registers;
    int parentId;

} Process;
