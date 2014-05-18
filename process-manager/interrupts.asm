                .186
                .model  tiny

                public  SetInterruptHandler
                public  GetInterruptHandler

                .code

; Input:
;     bx - interrupt number
;     cx - handler segment
;     ax - handler offset
; No output
SetInterruptHandler:

                push    ds

                push    word ptr 0x0000
                pop     ds

                shl     bx, 2

                pushf
                cli
                mov     word ptr [ bx ], ax
                mov     word ptr [ bx + 2 ], cx
                popf

                shr     bx, 2

                pop     ds
                ret

; end of SetInterruptHandler

; Input:
;     bx - interrupt number
; Output:
;     cx - handler segment
;     ax - handler offset
GetInterruptHandler:

                push    ds

                push    word ptr 0x0000
                pop     ds

                shl     bx, 2

                mov     ax, word ptr [ bx ]
                mov     cx, word ptr [ bx + 2 ]

                shr     bx, 2

                pop     ds
                ret

; end of GetInterruptHandler

                end
