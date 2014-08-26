// todo: убрать эти заголовочные файлов, реализовав необходимые обертки в mocks.h
#include <io.h>
#include <fcntl.h>

#include <common.h>
#include "memory.h"
#include "process.h"
#include "process-pool.h"

#include "timer.h"
#include "mocks.h"
#include "context.h"
#include "syscalls.h"

// todo: вынести такие константы отовсюду в отдельный заголовочный файл
#define _DEFAULT_STACK_SIZE 2048

extern Process *CreateProcess( char *pExecutablePath, int parameter )
{
    int fileHandle;
    MemoryBlockSize executableSize, segmentSize;
    Segment segment;
    Process *pProcess;

    pProcess = ProcessPool_CreateProcess();
    if ( pProcess == NULL ) {
        return NULL;
    }

    fileHandle = open( pExecutablePath, O_RDONLY | O_BINARY );

    executableSize = lseek( fileHandle, 0, SEEK_END );
    lseek( fileHandle, 0, SEEK_SET );

    // todo: реализовать поддержку формата COM-файлов
    segmentSize = executableSize + _DEFAULT_STACK_SIZE;
    pProcess->pEntryPoint = AllocateFarMemory( segmentSize );
    FarReadFromFile( fileHandle, pProcess->pEntryPoint, executableSize );

    close( fileHandle );

    segment = GetFpSegment( pProcess->pEntryPoint );
    pProcess->pDataSegment = MakeFp( segment, 0 );
    pProcess->pStackTop = MakeFp( segment, segmentSize - 1 );
    pProcess->parametersCount = 1;
    pProcess->parameters[ 0 ] = parameter;
    InitProcessContext( pProcess );
    pProcess->state = ProcessState_Ready;

    return pProcess;
}

extern void TerminateProcess()
{
    Process *pProcess = ProcessPool_GetActiveProcess();
    FreeFarMemory( pProcess->pEntryPoint );
    ProcessPool_DestroyActiveProcess();
    ProcessPool_ChooseProcessAndActivate();
}

// todo: вынести в загрузчик/инициализатор
extern void main()
{
    int i;

    InitKernelContext();
    ProcessPool_Init();
    InitSyscalls();

    for ( i = 0; i < 3; i++ ) {
        //CreateProcess( "randstr", 24 - i );
    }
    //CreateProcess( "worm", 0 );
    CreateProcess( "hw", 0 );

    InitTimer();

    // hack: В дальнейшем надо сделать или отдельный процесс, который будет
    // выключать процессор (аналог "Бездействия системы" в Windows), или делать
    // это в пуле процессов или где-то еще, когда нет ни одного готового процесса.
    asm {
        sti
        hlt
    }
}
