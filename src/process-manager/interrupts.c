#include <common.h>
#include "interrupts.h"
#include "architecture.h"
#include "memory.h"

static Byte _savedIrqMask;

// todo: Попробовать написать такую реализацию, которая не требовала бы
// реализовывать для каждого обработчика функцию-обертку, переключающую
// контекст и вызывающую ReturnFromIrqHandler в конце (только для обработчиков
// аппаратных прерываний, см. _TimerInterruptHandler).
// T.е. сюда должен передаваться указатель на функцию, содержащую голую логику
// обработки прерывания, все остальное должно добавляться само. (Посмотреть как
// это сделано в MINIX.)
extern void SetInterruptHandler( unsigned short number, void *pHandler )
{
    void far * far *ppVector = MakeFp( 0, number * 4 );
    asm {
        pushf
        cli
    }
    *ppVector = MakeFp( GetCurrentCodeSegment(), pHandler );
    asm {
        popf
    }
}

extern void far * GetInterruptHandler( unsigned short number )
{
    void far * far *ppVector = MakeFp( 0, number * 4 );
    return *ppVector;
}

extern void naked DisableIrqs()
{
    asm {
        in al, PIC1_PORT2
        mov _savedIrqMask, al

        // DEBUG
        or al, 1
        // mov al, PIC_OCW1_DISABLE_ALL_IRQ

        out PIC1_PORT2, al
        ret
    }
}

extern void naked RestoreIrqs()
{
    asm {
        mov al, _savedIrqMask
        out PIC1_PORT2, al
        ret
    }
}
