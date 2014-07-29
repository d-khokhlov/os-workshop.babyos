#include "common.h"
#include "process.h"

extern ProcessId ProcessId_GetNew()
{
    static ProcessId nextProcessId = ProcessId_Min;

    ProcessId newProcessId = nextProcessId;
    if ( nextProcessId < ProcessId_Max ) {
        nextProcessId++;
    } else {
        nextProcessId = ProcessId_Min;
    }
    return newProcessId;
}

extern void Process_Init( Process *pProcess )
{
    pProcess->id = ProcessId_GetNew();
    pProcess->state = ProcessState_Blocked;
    pProcess->blockReason = ProcessBlockReason_Initialization;
    pProcess->pEntryPoint = NULL;
    pProcess->pDataSegment = NULL;
    pProcess->pStackTop = NULL;
    pProcess->parametersCount = 0;
    // pNext не трогаем, т.к. процесс может уже находиться в какой-либо очереди
    // (вообще говоря за изменение pNext отвечает ProcessQueue).
}

extern void Process_Clear( Process *pProcess )
{
    pProcess->id = ProcessId_None;
    pProcess->state = ProcessState_Empty;
    pProcess->blockReason = ProcessBlockReason_NotBlocked;
    pProcess->pEntryPoint = NULL;
    pProcess->pDataSegment = NULL;
    pProcess->pStackTop = NULL;
    pProcess->parametersCount = 0;
    // pNext не трогаем (см. Init).
}
