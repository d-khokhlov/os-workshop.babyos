#include <common.h>
#include "interrupts.h"
#include "architecture.h"
#include "memory.h"

// todo: Попробовать написать такую реализацию, которая не требовала бы
// реализовывать для каждого обработчика функцию-обертку, переключающую
// контекст и вызывающую EndInterrupt в конце (см. _TimerInterruptHandler).
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
