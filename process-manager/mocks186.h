#pragma once

#include <io.h>
#include "registers186.h"

typedef SegmentRegister SegmentAddress;
typedef CommonRegister OffsetAddress;
typedef OffsetAddress NearMemorySize;
typedef long FarMemorySize;

typedef struct FarAddress
{
    OffsetAddress offset;
    SegmentAddress segment;
} FarAddress;

SegmentAddress _cdecl AllocateFarMemory( FarMemorySize size );
NearMemorySize _cdecl FarReadFromFile( int handle, FarAddress buffer,
    NearMemorySize count );
