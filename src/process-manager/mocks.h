#pragma once

#include "memory.h"

extern NearMemorySize FarReadFromFile( int handle, void far *pBuffer,
    NearMemorySize count );

extern unsigned int OutByteToPort( unsigned int port, unsigned int value );
