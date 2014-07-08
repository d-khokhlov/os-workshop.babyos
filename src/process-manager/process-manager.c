#include <io.h>
#include <fcntl.h>
#include "common.h"
#include "clock.h"
#include "process.h"
#include "registers.h"
#include "mocks.h"
#include "context.h"
#include "syscalls.h"

#define _MAX_PROCESSES_COUNT 20
Process _processes[ _MAX_PROCESSES_COUNT ];
ProcessId _activeProcessId;

#define DEFAULT_STACK_SIZE 2048

void _InitProcessManager()
{
    int id;
    for ( id = 0; id < _MAX_PROCESSES_COUNT; id++ ) {
        _processes[ id ].state = ProcessState_Null;
    }
    _activeProcessId = ProcessId_None;
}

ProcessId _GetNextProcessId( ProcessId id )
{
    if ( ( 0 <= id ) && ( id <= _MAX_PROCESSES_COUNT - 2 ) ) {
        return id + 1;
    } else {
        return 0;
    }
}

ProcessId _GetNextReadyProcess( ProcessId id )
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

ProcessId _GetFirstNullProcess()
{
    ProcessId id;
    for ( id = 0; id < _MAX_PROCESSES_COUNT; id++ ) {
        if ( _processes[ id ].state == ProcessState_Null ) {
            return id;
        }
    }
    return ProcessId_None;
}

ProcessId syscall CreateProcess( char *executablePath, int parameter )
{
    int fileHandle;
    NearMemorySize executableSize, segmentSize;
    FarAddress entryPoint;
    ProcessId id;
    Process *pProcess;

    id = _GetFirstNullProcess();
    if ( id == ProcessId_None ) {
        return ProcessId_None;
    }
    pProcess = &_processes[ id ];

    fileHandle = open( executablePath, O_RDONLY | O_BINARY );

    lseek( fileHandle, 0, SEEK_END );
    executableSize = tell( fileHandle );
    lseek( fileHandle, 0, SEEK_SET );

    segmentSize = executableSize + DEFAULT_STACK_SIZE;
    entryPoint.segment = AllocateFarMemory( segmentSize );
    entryPoint.offset = 0;
    FarReadFromFile( fileHandle, entryPoint, executableSize );

    close( fileHandle );

    pProcess->registers.cs = entryPoint.segment;
    pProcess->registers.ip = entryPoint.offset;
    pProcess->registers.ds = entryPoint.segment;
    pProcess->registers.es = entryPoint.segment;
    pProcess->registers.ss = entryPoint.segment;
    pProcess->registers.sp = segmentSize - 1;
    pProcess->parameters[ 0 ] = parameter;
    InitProcessContext( pProcess );
    pProcess->state = ProcessState_Ready;

    return id;
}

ProcessId ChooseProcessToActivate()
{
    ProcessId nextId = _GetNextReadyProcess( _activeProcessId );
    if ( nextId == ProcessId_None ) {
        return _activeProcessId;
    } else {
        return nextId;
    }
}

void ActivateProcess( ProcessId id )
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

Registers *GetActiveProcessRegisters()
{
    if ( _activeProcessId == ProcessId_None ) {
        return NULL;
    } else {
        return &( _processes[ _activeProcessId ].registers );
    }
}

void main()
{
    char c;
    int i;

    InitKernelContext();
    InitSyscalls();
    _InitProcessManager();

    for ( i = 0; i < 1; i++ ) {
        CreateProcess( "randstr", 24 - i );
    }
    CreateProcess( "worm", 0 );

    InitClock();

    read( STDIN_FILENO, &c, 1 );
}
