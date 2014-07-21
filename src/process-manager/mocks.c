#include "common.h"
#include "mocks.h"
#include <dos.h>
#include <conio.h>

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

extern NearMemorySize FarReadFromFile( int handle, void far *pBuffer,
    NearMemorySize count )
{
    NearMemorySize readCount;
    if ( _dos_read( handle, pBuffer, count, &readCount ) != 0 ) {
        return 0;
    }
    return readCount;
}

extern unsigned int OutByteToPort( unsigned int port, unsigned int value )
{
    return outp( port, value );
}
