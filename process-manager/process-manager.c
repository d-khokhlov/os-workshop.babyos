#include <io.h>
#include <fcntl.h>
#include "common.h"
#include "clock.h"
#include "process.h"
#include "registers.h"
#include "mocks.h"
#include "context.h"

#define _MAX_PROCESSES_COUNT 10
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

ProcessId CreateProcess( char *executablePath, int parameter )
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

void _Execute( char *executablePath, int parameter )
{
    unsigned long newCodeAddress = 0;

    __asm {

        // Открыть исполняемый файл на чтение
        mov     ah, 0x3D;
        mov     al, 10100000b;
        mov     dx, executablePath;
        mov     cl, 0x00;
        int     0x21;

        // Сохранить дескриптор открытого файла
        mov     bx, ax;

        // Вычислить сегментный адрес для загружаемого процесса
        mov     cx, ds:0x0002;
        sub     cx, 0x1000;

        // Инициализировать сегментные регистры данных для загружаемого процесса
        mov     ds, cx;
        mov     es, cx;

        // Сохранить указатель базы стекового кадра текущего процесса
        push    bp;

        // Забрать переданный параметр из стека, т.к. далее стек будет утерян
        mov     cx, parameter;

        // Запомнить указатель стека текущего процесса
        mov     dx, sp;

        // Инициализировать стек для загружаемого процесса
        mov     ax, ds;
        mov     ss, ax;
        mov     sp, 0xFFFE;

        // Сохранить в стек загружаемого процесса адрес стека текущего процесса
        push    dx;
        push    cs;

        // Передать параметр загружаемому процессу через его стек
        push    cx;

        // Загрузить файл в целевой сегмент
        mov     ah, 0x3F;
        mov     cx, 0xFFFF;
        mov     dx, 0x0000;
        int     0x21;

        // Закрыть файл
        mov     ah, 0x3E;
        int     0x21;

        // Передать управление загруженному процессу
        mov     word ptr newCodeAddress, 0x0000;
        mov     word ptr newCodeAddress + 2, ds;
        call    dword ptr newCodeAddress;

        // Восстановить стек текущего процесса
        pop     ax;
        pop     bx;
        mov     ss, ax;
        mov     sp, bx;

        // Восстановить стековый кадр текущего процесса
        pop     bp;

        // Восстановить сегментные регистры текущего процесса
        mov     cx, cs;
        mov     ds, cx;
        mov     es, cx;
    }
}

void main()
{
    char c;

    InitKernelContext();

    _InitProcessManager();
    CreateProcess( "randstr", 10 );
    CreateProcess( "randstr", 12 );
    CreateProcess( "randstr", 14 );
    CreateProcess( "randstr", 16 );
    CreateProcess( "randstr", 18 );
    CreateProcess( "randstr", 20 );

    InitClock();

    read( STDIN_FILENO, &c, 1 );
}
