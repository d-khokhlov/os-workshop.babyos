#pragma once

typedef enum ProcessState
{
    ProcessState_Empty,
    ProcessState_Ready,
    ProcessState_Active,
    ProcessState_Blocked,
} ProcessState;

typedef enum ProcessBlockReason
{
    ProcessBlockReason_NotBlocked,
    ProcessBlockReason_Initialization,
} ProcessBlockReason;

#define ProcessId_None -1
#define ProcessId_Min 0
#define ProcessId_Max ( ( 1 << ( sizeof( ProcessId ) * 8 - 1 ) ) - 1 )
typedef int ProcessId;
extern ProcessId ProcessId_GetNew();

#define ProcessParameters_MaxCount 1
typedef int ProcessParameters[ ProcessParameters_MaxCount ];

typedef struct Process
{
    ProcessId id;
    ProcessState state;
    ProcessBlockReason blockReason;
    void far *pEntryPoint;
    void far *pDataSegment;
    void far *pStackTop;
    int parametersCount;
    ProcessParameters parameters;
    struct Process *pNext;
} Process;
extern void Process_Init( Process *pProcess );
extern void Process_Clear( Process *pProcess );
