#pragma once

#include "architecture.h"

typedef SegmentSelector Segment;
typedef MachineWord Offset;
typedef MachineWord MemoryBlockSize;

#define GetFpSegment( farPointer ) \
    ( * (Segment *) ( ( (char *) &( farPointer ) ) + sizeof( Offset ) ) )

#define GetFpOffset( farPointer ) ( (Offset) ( farPointer ) )

#define MakeFp( segment, offset ) \
    ( ( (Segment) ( segment ) ) :> ( (Offset)( offset ) ) )

extern void far * AllocateFarMemory( MemoryBlockSize size );
extern Segment GetCurrentCodeSegment();
