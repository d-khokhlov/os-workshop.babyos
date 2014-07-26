#include "memory.h"
#include <dos.h>

extern void far * AllocateFarMemory( FarMemorySize size )
{
    unsigned int sizeInParagraphs;
    SegmentAddress segment;

    sizeInParagraphs = size >> 4;
    if ( ( size & 0x0F ) > 0 ) {
        sizeInParagraphs++;
    }

    if ( _dos_allocmem( sizeInParagraphs, (void *) &segment ) != 0 ) {
        return NULL;
    }
    return MakeFp( segment, 0 );
}
