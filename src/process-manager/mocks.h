#pragma once

#include "registers.h"
#include "common.h"

// todo: возможно вынести все до метки (1) в common.h или еще куда?

// todo: может объявить как __segment?
typedef SegmentRegister SegmentAddress;
typedef CommonRegister OffsetAddress;

// todo: добавить _Pack
typedef struct FarAddress
{
    OffsetAddress offset;
    SegmentAddress segment;
} FarAddress;

// todo: переименовать (т.к. можно понять как "общий объем ближней/дальней памяти")
typedef OffsetAddress NearMemorySize;
// todo: см. выше + изменить тип на OffsetAddress (может тогда и вообще убрать)
typedef unsigned long FarMemorySize;

// todo: переделать с использованием FarAddress
#define GetFpSegment( farPointer ) \
    ( (SegmentAddress) ( (unsigned long) (void far*) ( farPointer ) >> 16 ) )
#define GetFpOffset( farPointer ) ( (OffsetAddress) ( farPointer ) )
#define MakeFp( segment, offset) \
    ( ( (SegmentAddress) ( segment ) ) :> ( (void near *)( offset ) ) )

// (1)

extern void far * asmcall AllocateFarMemory( FarMemorySize size );

extern NearMemorySize FarReadFromFile( int handle, void far *buffer,
    NearMemorySize count );
