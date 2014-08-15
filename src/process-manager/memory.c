#include <common.h>
#include "memory.h"
#include <dos.h>

extern void far * AllocateFarMemory( MemoryBlockSize size )
{
    unsigned int sizeInParagraphs;

    // ������ �������� ��� Segment (unsigned short), �.�. _dos_allocmem
    // ��������� ����� ������ unsigned int-�.
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
    // hack: ������������� ��������� ������ ����� �������� warning-� �����������
    SegmentRegister csRegister = 0;
    asm {
        mov csRegister, cs
    }
    return csRegister;
}
