#include <common.h>
#include "process-queue.h"
#include "process.h"

extern void ProcessQueue_Init( ProcessQueue *pQueue )
{
    pQueue->pFirst = NULL;
    pQueue->pLast = NULL;
}

extern int ProcessQueue_IsEmpty( ProcessQueue *pQueue )
{
    return ( pQueue->pFirst == NULL );
}

extern void ProcessQueue_Enqueue( ProcessQueue *pQueue, Process *pProcess )
{
    if ( ProcessQueue_IsEmpty( pQueue ) ) {
        pQueue->pFirst = pProcess;
        pQueue->pLast = pProcess;
        pProcess->pNext = pProcess;
    } else {
        pProcess->pNext = pQueue->pFirst;
        pQueue->pLast->pNext = pProcess;
        pQueue->pLast = pProcess;
    }
}

extern Process *ProcessQueue_Dequeue( ProcessQueue *pQueue )
{
    if ( ProcessQueue_IsEmpty( pQueue ) ) {
        return NULL;
    } else {
        Process *removed = pQueue->pFirst;
        if ( pQueue->pLast == removed ) {
            pQueue->pFirst = NULL;
            pQueue->pLast = NULL;
        } else {
            pQueue->pFirst = removed->pNext;
            pQueue->pLast->pNext = removed->pNext;
        }
        removed->pNext = NULL;
        return removed;
    }
}

extern void ProcessQueue_Requeue( ProcessQueue *pQueue )
{
    if ( !ProcessQueue_IsEmpty( pQueue ) ) {
        pQueue->pLast = pQueue->pFirst;
        pQueue->pFirst = pQueue->pFirst->pNext;
    }
}
