#pragma once

#include "memory.h"

extern MemoryBlockSize FarReadFromFile( int handle, void far *pBuffer,
    MemoryBlockSize count );

extern unsigned int OutByteToPort( unsigned int port, unsigned int value );
