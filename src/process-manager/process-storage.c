#include "process-storage.h"
#include "process.h"

extern void ProcessStorage_Init( ProcessStorage storage )
{
    int i;
    for ( i = 0; i < ProcessStorage_Capacity; i++ ) {
        Process_Clear( &storage[ i ] );
    }
}
