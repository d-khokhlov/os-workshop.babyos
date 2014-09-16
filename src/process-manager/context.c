#include <common.h>
#include "context.h"
#include "process.h"
#include "process-pool.h"
#include "memory.h"
#include "architecture.h"
#include "timer.h"

#define _DEFAULT_FLAGS_VALUE 0x7202
#define _KERNEL_STACK_SIZE 2048

// hack: Ассемблер, встроенный в Watcom C, не умеет использовать члены структур
// из C. Поэтому в качестве типа этих двух переменных не получаеся использовать
// какую-нибудь абстрактную структуру для хранения контекста.
// todo: Единый стек ядра, сбрасываемый при каждом переходе в ядро,
// не обеспечивает повторной входимости.
static void far *_pKernelStackTop;

static void far *_pProcessStackTop;

static void *_pReturnPoint;

static void _SaveProcessContext()
{
    Process *pActiveProcess = ProcessPool_GetActiveProcess();
    if ( pActiveProcess != NULL ) {
        pActiveProcess->pStackTop = _pProcessStackTop;
    }
}

static void _LoadProcessContext()
{
    Process *pActiveProcess = ProcessPool_GetActiveProcess();
    if ( pActiveProcess != NULL ) {
        _pProcessStackTop = pActiveProcess->pStackTop;
        pActiveProcess->pStackTop = NULL;
    }
}

extern void naked SwitchContextToKernel()
{
    asm {
        pop cs : _pReturnPoint

        pusha
        push ds
        push es

        // hack: Ядро не повторно входимое, поэтому отключаем таймер, чтобы
        // исключить повторный вход.
        // В дальнейшем, когда не будут использоваться функции DOS, скорее всего
        // можно будет делать простой cli или вообще ничего не далать, т.к. флаг
        // IF сбрасывается при вызове обработчика аппаратного прерывания (однако
        // мы входим в ядро не только по аппаратным прерываниям).
        // Сейчас где-то внутри DOS функций (используются в некоторых системных
        // вызовах) флаг IF устанавливается снова. Поэтому приходится запрещать
        // прерывания на уровне контроллера прерываний.
        call DisableTimer

        // hack: Используется факт компиляции системы в формат COM (код и данные
        // в одном сегменте).
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

        // Включаем таймер, т.к. с этого момента можно повторно входить в ядро
        // (см. SwitchContextToKernel).
        call EnableTimer

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
        GetFpOffset( pStackTop ) & (Offset) ( ~1 ) );

    for ( i = pProcess->parametersCount - 1; i >= 0; i-- ) {
        _PushToStack( pStackTop, int, pProcess->parameters[ i ] );
    }

    _PushToStack( pStackTop, FlagsRegister, _DEFAULT_FLAGS_VALUE );
    _PushToStack( pStackTop, void far *, pProcess->pEntryPoint );

    pProcess->pStackTop = pStackTop;
    for ( i = 0; i < COMMON_REGISTERS_COUNT; i++ ) {
        _PushToStack( pStackTop, CommonRegister, 0 );
    }
    ( ( CommonRegister far * ) pStackTop )[ POPA_STACK_POINTER_INDEX ] =
        GetFpOffset( pProcess->pStackTop );

    _PushToStack( pStackTop, SegmentRegister,
        GetFpSegment( pProcess->pDataSegment ) );
    _PushToStack( pStackTop, SegmentRegister,
        GetFpSegment( pProcess->pDataSegment ) );

    pProcess->pStackTop = pStackTop;
}

#undef _PushToStack

extern void InitKernelContext()
{
    //void far *pStack = AllocateFarMemory( _KERNEL_STACK_SIZE );

    // Вершина стека ядра изначально указывает на следующий за выделенным
    // сегментом байт. Однако это не приведет к ошибкам, т.к. первой операцией
    // со стеком будет запись, которая первым делом сместит вершину на размер
    // записываемых данных, или резервирование места, которое тоже сместит вершину.
    //_pKernelStackTop = MakeFp( GetFpSegment( pStack ), _KERNEL_STACK_SIZE );

    asm {
        // hack: Используем текущий стек для ядра. Удобно, когда стек расположен
        // в том же сегменте, что и данные. Тем более, что текущая система
        // собирается в формат COM и использует именно такую модель памяти, и ее
        // нарушение приводит к ошибкам (например при копировании строк из
        // сегмента данных пользовательского процесса в локальную переменную
        // какой-либо функции ядра). Потом, когда будет настоящий загрузчик ядра
        // мы сами будем выделять этот сегмент и сможем управлять размером стека.
        mov word ptr _pKernelStackTop + 2, ss
        mov word ptr _pKernelStackTop, sp
    }
}
