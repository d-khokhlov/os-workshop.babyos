                .model  tiny
                .186

                public  C AllocateFarMemory
                public  C FarReadFromFile

                .code

; SegmentAddress _cdecl AllocateFarMemory( FarMemorySize size );
AllocateFarMemory:
                push    bp
                mov     bp, sp
                push    bx

                mov     bx, [ bp + 4 ]
                mov     ax, bx
                shr     bx, 4

                and     ax, 0x000F
                jz      @addHighWord
                inc     bx

@addHighWord:
                mov     ax, [ bp + 6 ]
                shl     ax, 12
                add     bx, ax

                mov     ah, 0x48
                int     0x21

                pop     bx
                pop     bp
                ret
; конец AllocateFarMemory

; NearMemorySize _cdecl FarReadFromFile( int handle, FarAddress buffer,
;     NearMemorySize count );
FarReadFromFile:
                push    bp
                mov     bp, sp
                push    bx
                push    cx
                push    ds
                push    dx

                mov     ah, 0x3F
                mov     bx, [ bp + 4 ]
                mov     cx, [ bp + 10 ]
                mov     dx, [ bp + 8 ]
                mov     ds, dx
                mov     dx, [ bp + 6 ]
                int     0x21

                pop     dx
                pop     ds
                pop     cx
                pop     bx
                pop     bp
                ret
; конец FarReadFromFile

                end
