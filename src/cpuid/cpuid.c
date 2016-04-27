#include <common.h>
#include <stdio.h>

#define _EFLAGS_ID_POSITION 21
#define _EFLAGS_ID_MASK 0x00200000

typedef struct CpuidLeaf
{
    unsigned long int eax, ebx, ecx, edx;
} CpuidLeaf;

static bool naked _CpuidIsSupported()
{
    asm {
        push bp
        pushfd // сохранить исходные значения флагов

        // Инвертировать флаг ID.
        pushfd
        mov bp, sp
        xor dword ptr [bp], _EFLAGS_ID_MASK
        popfd

        // Проверить, изменился ли флаг ID на самом деле.
        pushfd
        pop eax
        mov bp, sp // на вершине стека сейчас находится исходная копия флагов
        xor eax, [bp] // выделить изменившиеся биты
        and eax, _EFLAGS_ID_MASK // оставить только бит ID
        shr eax, _EFLAGS_ID_POSITION // вычислить возвращаемое значение

        popfd // восстановить исходные значения флагов
        pop bp
        ret
    }
}

static void _GetCpuidLeaf( unsigned long int leafId, CpuidLeaf *pLeaf )
{
    asm {
        push ebx
        push ecx
        push edx
        push di

        mov eax, leafId
        cpuid

        mov di, pLeaf
        mov [di], eax
        mov [di + 4], ebx
        mov [di + 8], ecx
        mov [di + 12], edx

        pop di
        pop edx
        pop ecx
        pop ebx
    }
}

__declspec( __watcall ) extern void *malloc( size_t size );
__declspec( __watcall ) extern void free( void *ptr );

extern void main()
{
    if ( _CpuidIsSupported() ) {

        CpuidLeaf *pLeaf = malloc( sizeof( CpuidLeaf ) );
        char vendorId[ 13 ];
        unsigned long int *pVendorIdComponent = ( unsigned long int * ) vendorId;

        _GetCpuidLeaf( 0x00, pLeaf );
        printf( "Max basic leaf: 0x%lX\n", pLeaf->eax );
        *pVendorIdComponent = pLeaf->ebx;
        *( pVendorIdComponent + 1 ) = pLeaf->edx;
        *( pVendorIdComponent + 2 ) = pLeaf->ecx;
        vendorId[ 12 ] = 0;
        printf( "Vendor identification string: %s\n", vendorId );

        free( pLeaf );

    } else {
        printf( "CPUID is not supported :(\n" );
    }

    printf( "\n" );
}
