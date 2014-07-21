#include "common.h"
#include "syscalls.h"
#include "context.h"
#include "interrupts.h"
#include "process-manager.h"

#define _SYSCALLS_COUNT 1

// todo: попробовать сделать это перечислением
typedef int _SyscallId;

// todo: как передавать параметры в системный вызов?
typedef struct _SyscallParameters
{
    _SyscallId number;
} _SyscallParameters;

typedef void( *_PSyscallHandler )( );

static _PSyscallHandler _pSyscallHandlers[ _SYSCALLS_COUNT ];

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

extern void InitSyscalls()
{
    _pSyscallHandlers[ 0 ] = &CreateProcess;
    SetInterruptHandler( INTERRUPT_SYSCALLS, &_SyscallInterruptHandler );
}
