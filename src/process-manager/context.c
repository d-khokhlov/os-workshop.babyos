#include "common.h"
#include "context.h"
#include "process.h"
#include "process-manager.h"
#include "memory.h"

#define _DEFAULT_FLAGS 0x7202
#define _KERNEL_STACK_SIZE 2048

// hack: Ассемблер, встроенный в Watcom C, не умеет использовать члены структур
// из C. Поэтому в качестве типа этих двух переменных не получаеся использовать
// какую-нибудь абстрактную структуру для хранения контекста.
static void far * _pKernelStackTop;
static void far *_pProcessStackTop;

static void *_pReturnPoint;

static void _SaveProcessContext()
{
    GetActiveProcess()->pStackTop = _pProcessStackTop;
}

static void _LoadProcessContext()
{
    _pProcessStackTop = GetActiveProcess()->pStackTop;
}

extern void naked SwitchContextToKernel()
{
    asm {
        pop cs : _pReturnPoint

        pusha
        push ds
        push es

        mov ax, cs
        mov ds, ax
        mov es, ax

        mov word ptr _pProcessStackTop + 2, ss
        mov word ptr _pProcessStackTop, sp
        mov ss, word ptr _pKernelStackTop + 2
        mov sp, word ptr _pKernelStackTop

        push _pReturnPoint
        jmp _SaveProcessContext
    }
}

extern void naked SwitchContextToProcess()
{
    asm {
        call _LoadProcessContext
        pop _pReturnPoint

        mov ss, word ptr _pProcessStackTop + 2
        mov sp, word ptr _pProcessStackTop

        pop es
        pop ds
        popa

        jmp cs : _pReturnPoint
    }
}

#define _PushToStack( pStackTop, type, value) \
    ( pStackTop ) = ( (char far *) ( pStackTop ) ) - sizeof( value ); \
    *( (type far *) ( pStackTop ) ) = ( value )

extern void InitProcessContext( Process *pProcess )
{
    int i;
    void far * pStackTop = pProcess->pStackTop;

    // Выбрать ближайший четный адрес для вершины стека (обнулить младший бит).
    pStackTop = MakeFp( GetFpSegment( pStackTop ),
        GetFpOffset( pStackTop ) & (OffsetAddress) ( ~1 ) );

    for ( i = MAX_PROCESS_PARAMETERS_COUNT - 1; i >= 0; i-- ) {
        _PushToStack( pStackTop, int, pProcess->parameters[ i ] );
    }

    _PushToStack( pStackTop, FlagsRegister, _DEFAULT_FLAGS );
    _PushToStack( pStackTop, void far *, pProcess->pEntryPoint );

    pProcess->pStackTop = pStackTop;
    for ( i = 0; i < COMMON_REGISTERS_COUNT; i++ ) {
        _PushToStack( pStackTop, CommonRegister, 0 );
    }
    ( ( CommonRegister far * ) pStackTop )[ POPA_STACK_POINTER_INDEX ] =
        GetFpOffset( pProcess->pStackTop );

    _PushToStack( pStackTop, SegmentAddress,
        GetFpSegment( pProcess->pDataSegment ) );
    _PushToStack( pStackTop, SegmentAddress,
        GetFpSegment( pProcess->pDataSegment ) );

    pProcess->pStackTop = pStackTop;
}

#undef _PushToStack

extern void InitKernelContext()
{
    _pKernelStackTop = AllocateFarMemory( _KERNEL_STACK_SIZE );
}
