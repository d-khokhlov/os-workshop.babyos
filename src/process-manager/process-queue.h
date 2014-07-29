#pragma once

#include "process.h"

typedef struct ProcessQueue
{
    Process *pFirst, *pLast;
} ProcessQueue;

extern void ProcessQueue_Init( ProcessQueue *pQueue );
extern int ProcessQueue_IsEmpty( ProcessQueue *pQueue );
extern void ProcessQueue_Enqueue( ProcessQueue *pQueue, Process *pProcess );
extern Process *ProcessQueue_Dequeue( ProcessQueue *pQueue );
extern void ProcessQueue_Requeue( ProcessQueue *pQueue );
