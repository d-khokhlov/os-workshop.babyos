#include <common.h>
#include <stdio.h>

#define _EFLAGS_ID_POSITION 21
#define _EFLAGS_ID_MASK 0x00200000

typedef struct _CpuidLeaf
{
    unsigned long int eax, ebx, ecx, edx;
} _CpuidLeaf;

typedef packed struct _CpuidVersionInformation
{
    unsigned int stepping : 4;
    unsigned int model : 4;
    unsigned int family : 4;
    unsigned int type : 2;
    unsigned int : 2;
    unsigned int extendedModel : 4;
    unsigned int extendedFamily : 8;
} _CpuidVersionInformation;

typedef packed struct _Cpuid01Ebx
{
    unsigned char brandIndex;
    unsigned char cacheLineSize;
    unsigned char maxLogicalProcessorsIds;
    unsigned char localApicId;
} _Cpuid01Ebx;

static char *_cpuidEcxFeatures[ 32 ] = {
    "SSE3",
    "PCLMULQDQ",
    "DTES64",
    "MONITOR",
    "DS-CPL",
    "VMX",
    "SMX",
    "EIST",
    "TM2",
    "SSSE3",
    "CNXT-ID",
    "SDBG",
    "FMA",
    "CMPXCHG16B",
    "xTPR",
    "PDCM",
    NULL,
    "PCID",
    "DCA",
    "SSE4.1",
    "SSE4.2",
    "x2APIC",
    "MOVBE",
    "POPCNT",
    "TSC-Deadline",
    "AESNI",
    "XSAVE",
    "OSXSAVE",
    "AVX",
    "F16C",
    "RDRAND",
    NULL,
};

static char *_cpuidEdxFeatures[ 32 ] = {
    "FPU",
    "VME",
    "DE",
    "PSE",
    "TSC",
    "MSR",
    "PAE",
    "MCE",
    "CX8",
    "APIC",
    NULL,
    "SEP",
    "MTRR",
    "PGE",
    "MCA",
    "CMOV",
    "PAT",
    "PSE-36",
    "PSN",
    "CLFSH",
    NULL,
    "DS",
    "ACPI",
    "MMX",
    "FXSR",
    "SSE",
    "SSE2",
    "SS",
    "HTT",
    "TM",
    NULL,
    "PBE",
};

static bool naked _CpuidIsSupported()
{
    asm {
        push bp
        pushfd // сохранить исходные значения флагов

        // Инвертировать флаг ID.
        pushfd
        mov bp, sp
        xor dword ptr [ bp ], _EFLAGS_ID_MASK
        popfd

        // Проверить, изменился ли флаг ID на самом деле.
        pushfd
        pop eax
        mov bp, sp // на вершине стека сейчас находится исходная копия флагов
        xor eax, [ bp ] // выделить изменившиеся биты
        and eax, _EFLAGS_ID_MASK // оставить только бит ID
        shr eax, _EFLAGS_ID_POSITION // вычислить возвращаемое значение

        popfd // восстановить исходные значения флагов
        pop bp
        ret
    }
}

static void _GetCpuidLeaf( unsigned long int leafId, _CpuidLeaf *pLeaf )
{
    asm {
        push ebx
        push ecx
        push edx
        push di

        mov eax, leafId
        cpuid

        mov di, pLeaf
        mov [ di ], eax
        mov [ di + 4 ], ebx
        mov [ di + 8 ], ecx
        mov [ di + 12 ], edx

        pop di
        pop edx
        pop ecx
        pop ebx
    }
}

static bool _PrintCpuidFeatures( unsigned long int flags, char *features[] )
{
    bool someFeaturesPrinted = FALSE;
    int index;
    for ( index = 0; index < 32; index++ ) {
        if ( flags & ( 1ul << index ) && features[ index ] != NULL ) {
            if ( someFeaturesPrinted ) {
                printf( ", " );
            }
            printf( features[ index ] );
            someFeaturesPrinted = TRUE;
        }
    }
    return someFeaturesPrinted;
}

extern void main()
{
    if ( _CpuidIsSupported() ) {

        _CpuidLeaf leaf;
        unsigned long int maxBasicLeaf, maxExtendedLeaf;

        char vendorId[ 13 ];
        unsigned long int *pVendorIdComponent = ( unsigned long int * ) vendorId;
        _GetCpuidLeaf( 0x00, &leaf );
        maxBasicLeaf = leaf.eax;
        printf( "Max basic leaf: 0x%02lX\n", maxBasicLeaf );
        *pVendorIdComponent = leaf.ebx;
        *( pVendorIdComponent + 1 ) = leaf.edx;
        *( pVendorIdComponent + 2 ) = leaf.ecx;
        vendorId[ 12 ] = 0;
        printf( "Vendor identification string: %s\n", vendorId );

        _GetCpuidLeaf( 0x80000000, &leaf );
        maxExtendedLeaf = leaf.eax;
        printf( "Max extended leaf: 0x%lX\n", maxExtendedLeaf );

        if ( 0x01 <= maxBasicLeaf ) {
            _CpuidVersionInformation *pVersionInfo;
            unsigned int family, model;
            _Cpuid01Ebx *pEbxInfo;

            _GetCpuidLeaf( 0x01, &leaf );
            pVersionInfo = ( _CpuidVersionInformation * ) &( leaf.eax );
            family = pVersionInfo->family;
            model = pVersionInfo->model;
            if ( family == 0x06 || family == 0x0F ) {
                model += (unsigned int)pVersionInfo->extendedModel << 4;
            }
            if ( family == 0x0F ) {
                family += (unsigned int)pVersionInfo->extendedFamily;
            }
            pEbxInfo = ( _Cpuid01Ebx * )&( leaf.ebx );

            printf( "Type: 0x%02X\n", (unsigned int)pVersionInfo->type );
            printf( "Family: 0x%02X\n", family );
            printf( "Model: 0x%02X\n", model );
            printf( "Stepping: 0x%02X\n", (unsigned int)pVersionInfo->stepping );
            printf( "Brand string index: %hhu\n", pEbxInfo->brandIndex );
            printf( "Cache line size: %u\n", (unsigned int)pEbxInfo->cacheLineSize * 8 );
            if ( leaf.edx & ( 1ul << 28 ) ) {
                printf( "Max logical processors IDs: %hhu\n", pEbxInfo->maxLogicalProcessorsIds );
            }
            printf( "Initial local APIC ID: %hhu\n", pEbxInfo->localApicId );
            printf( "Features: " );
            if ( _PrintCpuidFeatures( leaf.edx, _cpuidEdxFeatures ) ) {
                printf( ", " );
            }
            _PrintCpuidFeatures( leaf.ecx, _cpuidEcxFeatures );
            printf( "\n" );
        }
    } else {
        printf( "CPUID is not supported\n" );
    }

    printf( "\n" );
}
