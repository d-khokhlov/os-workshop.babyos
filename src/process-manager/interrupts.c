#include <common.h>
#include "interrupts.h"
#include "architecture.h"
#include "memory.h"
#include "mocks.h"

#ifdef interrupt
#undef interrupt
#endif

#define _PIC_BIT_OFFSET_IN_IRQ 3
#define _CLEAR_PIC_IN_IRQ_MASK ( ( 1 << _PIC_BIT_OFFSET_IN_IRQ ) - 1 )

static unsigned short _picsFirstPorts[] = { PIC1_PORT1, PIC2_PORT1 };

// todo: Попробовать написать такую реализацию, которая не требовала бы
// реализовывать для каждого обработчика функцию-обертку, переключающую
// контекст и вызывающую ReturnFromIrqHandler в конце (только для обработчиков
// аппаратных прерываний, см. _TimerInterruptHandler).
// T.е. сюда должен передаваться указатель на функцию, содержащую голую логику
// обработки прерывания, все остальное должно добавляться само. (Посмотреть как
// это сделано в MINIX.)
extern void SetInterruptHandler( unsigned short interrupt, void *pHandler )
{
    void far * far *ppVector = MakeFp( 0, interrupt * FAR_POINTER_SIZE );
    asm {
        pushf
        cli
    }
    *ppVector = MakeFp( GetCurrentCodeSegment(), pHandler );
    asm {
        popf
    }
}

extern void far * GetInterruptHandler( unsigned short interrupt )
{
    void far * far *ppVector = MakeFp( 0, interrupt * FAR_POINTER_SIZE );
    return *ppVector;
}

extern void DisableIrq( unsigned char irq )
{
    unsigned short picPort = _picsFirstPorts[ irq >> _PIC_BIT_OFFSET_IN_IRQ ];
    unsigned char disabledIrqMask = ReadByteFromPort( picPort );
    disabledIrqMask |= 1 << ( irq & _CLEAR_PIC_IN_IRQ_MASK );
    WriteByteToPort( picPort, disabledIrqMask );
}

extern void EnableIrq( unsigned char irq )
{
    unsigned short picPort = _picsFirstPorts[ irq >> _PIC_BIT_OFFSET_IN_IRQ ];
    unsigned char disabledIrqMask = ReadByteFromPort( picPort );
    disabledIrqMask &= ~( 1 << ( irq & _CLEAR_PIC_IN_IRQ_MASK ) );
    WriteByteToPort( picPort, disabledIrqMask );
}
