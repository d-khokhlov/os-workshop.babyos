#include "mocks.h"
#include <dos.h>
#include <conio.h>

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
