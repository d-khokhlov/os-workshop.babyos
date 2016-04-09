#pragma once

#include <common.h>
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
extern void FreeFarMemory( void far *pBlock );
extern Segment GetCurrentCodeSegment();
extern bool CopyFarStringToNear( char far *source, char *destination,
    int destinationCapacity );
