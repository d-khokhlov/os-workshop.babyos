#pragma once

#include <io.h>
#include "common.h"
#include "registers.h"

typedef SegmentRegister SegmentAddress;
typedef CommonRegister OffsetAddress;
typedef OffsetAddress NearMemorySize;
typedef long FarMemorySize;

typedef struct FarAddress
{
    OffsetAddress offset;
    SegmentAddress segment;
} FarAddress;

SegmentAddress public AllocateFarMemory( FarMemorySize size );
NearMemorySize public FarReadFromFile( int handle, FarAddress buffer,
    NearMemorySize count );
