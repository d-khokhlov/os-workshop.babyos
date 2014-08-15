#pragma once

// hack: подключается для того, чтобы пользовательские библиотеки системных
// вызовов могли использовать INTERRUPT_SYSCALLS, не подключая данный файл напрямую.
#include "interrupts.h"

// hack: Все это нельзя сделать перечислением, т.к. ассемблер, встроенный в
// Watcom C, не умеет использовать элементы перечислений, что требуется в
// пользовательских библиотеках системных вызовов.
typedef int SyscallId;
#define SyscallId_CreateProcess 0
#define SyscallId_TerminateProcess 1

#define SYSCALLS_COUNT 2

extern void InitSyscalls();
