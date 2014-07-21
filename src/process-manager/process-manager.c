#include <io.h>
#include <fcntl.h>
#include "common.h"
#include "timer.h"
#include "process.h"
#include "mocks.h"
#include "context.h"
#include "syscalls.h"

#define _MAX_PROCESSES_COUNT 20
static Process _processes[ _MAX_PROCESSES_COUNT ];
static ProcessId _activeProcessId;

#define _DEFAULT_STACK_SIZE 2048

static void _InitProcessManager()
{
    int id;
    for ( id = 0; id < _MAX_PROCESSES_COUNT; id++ ) {
        _processes[ id ].state = ProcessState_Null;
    }
    _activeProcessId = ProcessId_None;
}

static ProcessId _GetNextProcessId( ProcessId id )
{
    if ( ( 0 <= id ) && ( id <= _MAX_PROCESSES_COUNT - 2 ) ) {
        return id + 1;
    } else {
        return 0;
    }
}

static ProcessId _GetNextReadyProcess( ProcessId id )
{
    ProcessId nextId;

    if ( id == ProcessId_None ) {
        if ( _processes[ 0 ].state == ProcessState_Ready ) {
            return 0;
        }
        id = 0;
    }

    nextId = _GetNextProcessId( id );

    while ( nextId != id ) {
        if ( _processes[ nextId ].state == ProcessState_Ready ) {
            return nextId;
        }
        nextId = _GetNextProcessId( nextId );
    }

    return ProcessId_None;
}

static ProcessId _GetFirstNullProcess()
{
    ProcessId id;
    for ( id = 0; id < _MAX_PROCESSES_COUNT; id++ ) {
        if ( _processes[ id ].state == ProcessState_Null ) {
            return id;
        }
    }
    return ProcessId_None;
}

extern ProcessId CreateProcess( char *pExecutablePath, int parameter )
{
    int fileHandle;
    NearMemorySize executableSize, segmentSize;
    SegmentAddress segmentAddress;
    ProcessId id;
    Process *pProcess;

    id = _GetFirstNullProcess();
    if ( id == ProcessId_None ) {
        return ProcessId_None;
    }
    pProcess = &( _processes[ id ] );

    fileHandle = open( pExecutablePath, O_RDONLY | O_BINARY );

    executableSize = lseek( fileHandle, 0, SEEK_END );
    lseek( fileHandle, 0, SEEK_SET );

    segmentSize = executableSize + _DEFAULT_STACK_SIZE;
    pProcess->pEntryPoint = AllocateFarMemory( segmentSize );
    FarReadFromFile( fileHandle, pProcess->pEntryPoint, executableSize );

    close( fileHandle );

    segmentAddress = GetFpSegment( pProcess->pEntryPoint );
    pProcess->pDataSegment = MakeFp( segmentAddress, 0 );
    pProcess->pStackTop = MakeFp( segmentAddress, segmentSize - 1 );
    pProcess->parameters[ 0 ] = parameter;
    InitProcessContext( pProcess );
    pProcess->state = ProcessState_Ready;

    return id;
}

extern ProcessId ChooseProcessToActivate()
{
    ProcessId nextId = _GetNextReadyProcess( _activeProcessId );
    if ( nextId == ProcessId_None ) {
        return _activeProcessId;
    } else {
        return nextId;
    }
}

extern void ActivateProcess( ProcessId id )
{
    if ( id != _activeProcessId ) {
        if ( _activeProcessId != ProcessId_None ) {
            _processes[ _activeProcessId ].state = ProcessState_Ready;
        }
        if ( id != ProcessId_None ) {
            _processes[ id ].state = ProcessState_Active;
        }
        _activeProcessId = id;
    }
}

extern Process *GetActiveProcess()
{
    if ( _activeProcessId == ProcessId_None ) {
        return NULL;
    } else {
        return &( _processes[ _activeProcessId ] );
    }
}

// todo: вынести в загрузчик/инициализатор
extern void main()
{
    char c;
    int i;

    InitKernelContext();
    InitSyscalls();
    _InitProcessManager();

    for ( i = 0; i < 3; i++ ) {
        CreateProcess( "randstr", 24 - i );
    }
    CreateProcess( "worm", 0 );

    InitTimer();

    read( STDIN_FILENO, &c, 1 );
}
