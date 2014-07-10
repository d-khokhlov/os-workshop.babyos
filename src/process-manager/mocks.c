#include <dos.h>
#include "mocks.h"
#include "common.h"

extern void far * asmcall AllocateFarMemory( FarMemorySize size )
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

extern NearMemorySize FarReadFromFile( int handle, void far *buffer,
    NearMemorySize count )
{
    NearMemorySize readCount;
    if ( _dos_read( handle, buffer, count, &readCount ) != 0 ) {
        return 0;
    }
    return readCount;
}
