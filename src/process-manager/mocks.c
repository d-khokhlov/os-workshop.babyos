#include "mocks.h"
#include <dos.h>
#include <conio.h>

extern MemoryBlockSize FarReadFromFile( int handle, void far *pBuffer,
    MemoryBlockSize count )
{
    MemoryBlockSize actualCount;
    if ( _dos_read( handle, pBuffer, count, &actualCount ) != 0 ) {
        return 0;
    }
    return actualCount;
}

extern unsigned int OutByteToPort( unsigned int port, unsigned int value )
{
    return outp( port, value );
}
