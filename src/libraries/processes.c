#include <common.h>
#include <processes.h>

// todo: сделать так, чтобы не нужно было использовать такой путь
#include "../process-manager/syscalls.h"

// hack: Используются особенности соглашения о вызове Open Watcom C.
// Параметры по возможности передаются в регистрах в таком порядке:
//     ax, dx, bx, cx.
// Результат возвращается в регистре ax.
// Подробное описание см. в "Open Watcom C/C++ User's Guide",
// раздел "16-bit Assembly Language Considerations",
// подраздел "16-bit: Calling Conventions for Non-80x87 Applications".


extern Handle naked CreateProcess( char *pExecutablePath, int parameter )
{
    asm {
        push di
        mov di, SyscallId_CreateProcess
        int INTERRUPT_SYSCALLS
        pop di
    }
}

extern void naked TerminateProcess()
{
    asm {
        push di
        mov di, SyscallId_TerminateProcess
        int INTERRUPT_SYSCALLS
        pop di
    }
}
