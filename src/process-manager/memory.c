#include <common.h>
#include "memory.h"
#include <dos.h>

extern void far * AllocateFarMemory( MemoryBlockSize size )
{
    unsigned int sizeInParagraphs;

    // Нельзя объявить как Segment (unsigned short), т.к. _dos_allocmem
    // принимает адрес именно unsigned int-а.
    unsigned int segment;

    sizeInParagraphs = size >> 4;
    if ( ( size & 0x0F ) > 0 ) {
        sizeInParagraphs++;
    }

    if ( _dos_allocmem( sizeInParagraphs, &segment ) != 0 ) {
        return NULL;
    }
    return MakeFp( segment, 0 );
}

extern Segment GetCurrentCodeSegment()
{
    // hack: инициализация добавлена только чтобы избежать warning-а компилятора
    SegmentRegister csRegister = 0;
    asm {
        mov csRegister, cs
    }
    return csRegister;
}
