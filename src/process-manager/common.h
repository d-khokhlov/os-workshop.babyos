#pragma once

#define NULL ( (void *) 0 )

#define asm __asm
#define naked __declspec( naked )

// todo: это все нужно убрать
#define public _cdecl
#define syscall _cdecl
#define asmcall _cdecl
