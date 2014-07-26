#pragma once

#include "registers.h"

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

extern void far * AllocateFarMemory( FarMemorySize size );
