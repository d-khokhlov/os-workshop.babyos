#pragma once

#include "memory.h"

extern MemoryBlockSize FarReadFromFile( int handle, void far *pBuffer,
    MemoryBlockSize count );

extern unsigned char ReadByteFromPort( unsigned int port );
extern void WriteByteToPort( unsigned int port, unsigned char value );
