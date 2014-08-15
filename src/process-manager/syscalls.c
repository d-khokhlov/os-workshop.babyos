#include <common.h>
#include "syscalls.h"
#include "context.h"
#include "interrupts.h"
#include "process-manager.h"

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
    _pSyscallHandlers[ 0 ] = (_PSyscallHandler) &CreateProcess;
    SetInterruptHandler( INTERRUPT_SYSCALLS, &_SyscallInterruptHandler );
}
