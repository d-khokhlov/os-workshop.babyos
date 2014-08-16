#include <common.h>
#include "syscalls.h"
#include "context.h"
#include "interrupts.h"
#include "architecture.h"
#include "memory.h"
#include "process-pool.h"
#include "process-manager.h"

#define _PATH_MAX_LENGTH 128

typedef void( *_PSyscallHandler )( );

static _PSyscallHandler _pSyscallHandlers[ SYSCALLS_COUNT ];

static void naked _SyscallInterruptHandler()
{
    asm {
        call SwitchContextToKernel
        // todo: Реализовать вызов реального обработчика и передача параметров.
        // Может быть реализовать эту функцию на C с модификатором interrupt?
        call SwitchContextToProcess
        iret
    }
}

// ax: char *pExecutablePath
// dx: int parameter
// return: Handler процесса
static void _SyscallHandler_CreateProcess()
{
    char executablePath[ _PATH_MAX_LENGTH ];
    ProcessContext far *pContext;

    pContext = ProcessPool_GetActiveProcess()->pStackTop;
    CopyFarStringToNear( MakeFp( pContext->ds, pContext->ax ), executablePath,
        _PATH_MAX_LENGTH );
    pContext->ax = (CommonRegister) CreateProcess( executablePath, pContext->dx );
}

// Нет параметров.
// Нет возвращаемого значения.
static void _SyscallHandler_TerminateProcess()
{
    TerminateProcess();
}

// hack: Порядок передачи аргументов в регистрах и возврата значения основан на
// Open Watcom C:
//     аргументы в ax, dx, bx, cx;
//     результат в ax.
// Дополнительно в регистре di передается идентификатор системного вызова.
// Подробное описание см. в "Open Watcom C/C++ User's Guide",
// раздел "16-bit Assembly Language Considerations",
// подраздел "16-bit: Calling Conventions for Non-80x87 Applications".

extern void InitSyscalls()
{
    _pSyscallHandlers[ SyscallId_CreateProcess ] =
        &_SyscallHandler_CreateProcess;
    _pSyscallHandlers[ SyscallId_TerminateProcess ] =
        &_SyscallHandler_TerminateProcess;

    SetInterruptHandler( INTERRUPT_SYSCALLS, &_SyscallInterruptHandler );
}
