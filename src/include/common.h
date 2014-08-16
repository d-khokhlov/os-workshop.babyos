#pragma once

typedef unsigned char bool;
#define FALSE ( (bool) 0 )
#define TRUE ( (bool) 1 )

#define NULL ( (void *) 0 )

#define asm __asm
#define naked __declspec( naked )
#define packed _Packed
