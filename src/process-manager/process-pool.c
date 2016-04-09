#include <common.h>
#include "process-pool.h"
#include "process.h"
#include "process-storage.h"
#include "process-queue.h"

typedef struct _ProcessPool
{
    ProcessStorage storage;
    ProcessQueue usedQueue;
    ProcessQueue emptyQueue;
    // В нормальном состоянии, если в системе есть активный процесс, то это
    // первый процесс в очереди. Однако при поиске другого процесса для
    // активации, активный процесс помещается в конец очереди и в общем случае,
    // может оказаться где-то в ее середине. Чтобы всегда иметь быстрый доступ
    // к активному процессу, храним отдельный указатель на него.
    Process *pActiveProcess;
} _ProcessPool;

static _ProcessPool _pool;

static void _ProcessPool_ActivateProcess( Process *pProcess )
{
    if ( pProcess != _pool.pActiveProcess ) {
        if ( _pool.pActiveProcess != NULL ) {
            _pool.pActiveProcess->state = ProcessState_Ready;
        }
        if ( pProcess != NULL ) {
            pProcess->state = ProcessState_Active;
        }
        _pool.pActiveProcess = pProcess;
    }
}

extern void ProcessPool_Init()
{
    int i;

    ProcessStorage_Init( _pool.storage );
    ProcessQueue_Init( &_pool.usedQueue );
    ProcessQueue_Init( &_pool.emptyQueue );

    for ( i = 0; i < ProcessStorage_Capacity; i++ ) {
        ProcessQueue_Enqueue( &_pool.emptyQueue, &_pool.storage[ i ] );
    }

    _pool.pActiveProcess = NULL;
}

extern void ProcessPool_ChooseProcessAndActivate()
{
    Process *pInitialFirst;

    if ( ProcessQueue_IsEmpty( &_pool.usedQueue ) ) {
        return;
    }

    pInitialFirst = _pool.usedQueue.pFirst;
    do {
        if ( _pool.usedQueue.pFirst->state == ProcessState_Ready ) {
            _ProcessPool_ActivateProcess( _pool.usedQueue.pFirst );
            return;
        }
        ProcessQueue_Requeue( &_pool.usedQueue );
    } while ( _pool.usedQueue.pFirst != pInitialFirst );
}

extern Process *ProcessPool_GetActiveProcess()
{
    return _pool.pActiveProcess;
}

extern Process *ProcessPool_CreateProcess()
{
    Process *pProcess = ProcessQueue_Dequeue( &_pool.emptyQueue );
    if ( pProcess != NULL ) {
        Process_Init( pProcess );
        ProcessQueue_Enqueue( &_pool.usedQueue, pProcess );
    }
    return pProcess;
}

extern bool ProcessPool_DestroyActiveProcess()
{
    Process *pProcess;

    if ( _pool.pActiveProcess == NULL ) {
        return FALSE;
    }

    _pool.pActiveProcess = NULL;
    pProcess = ProcessQueue_Dequeue( &_pool.usedQueue );
    Process_Clear( pProcess );
    ProcessQueue_Enqueue( &_pool.emptyQueue, pProcess );

    return TRUE;
}
