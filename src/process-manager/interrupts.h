#pragma once

#include "common.h"

// PIC - Programmable Interrupt Controller
#define PIC1_PORT1 0x20
#define PIC1_PORT2 0x21

#define PIC1_INTERRUPT_BASE 0x08

// EOI - End Of Interrupt
#define PIC_OCW2_NONSPECIFIC_EOI 0x20

// todo: Константы INTERRUPT_x можно объединить в перечисление и использовать
// его в функциях в качестве типа параметра interrupt. Но тогда желательно
// определить в перечислении все 256 прерываний, что как-то глупо.
#define INTERRUPT_IRQ0 PIC1_INTERRUPT_BASE
#define INTERRUPT_IRQ1 ( PIC1_INTERRUPT_BASE + 1 )

#define INTERRUPT_TIMER INTERRUPT_IRQ0
#define INTERRUPT_KEYBOARD INTERRUPT_IRQ1

#define INTERRUPT_SYSCALLS 0x90

// Нужно только для EndInterrupt
#define PIC1
#define PIC2

// pic: PIC1 или PIC2
#define EndInterrupt( pic ) \
    asm{ push ax }\
    asm{ mov al, PIC_OCW2_NONSPECIFIC_EOI }\
    asm{ out pic ## _PORT1, al }\
    asm{ pop ax }\
    asm{ iret }

extern void asmcall SetInterruptHandler( unsigned short interrupt, void *pHandler );
extern void far * asmcall GetInterruptHandler( unsigned short interrupt );
