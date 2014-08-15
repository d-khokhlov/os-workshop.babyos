#pragma once

// hack: подключается для того, чтобы пользовательские библиотеки системных
// вызовов могли использовать INTERRUPT_SYSCALLS, не подключая данный файл напрямую.
#include "interrupts.h"

typedef enum SyscallId
{
    SyscallId_CreateProcess,
    SyscallId_TerminateProcess,
    // Этот элемент должен быть всегда последним
    SyscallId_Max
} SyscallId;

extern void InitSyscalls();
