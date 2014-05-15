                .186
                .model  tiny

                public  setIrqHandler
                public  oldHandler

                .code

                cli
                hlt

; bx - IRQ number
; ax - handler offset
; cx - handler segment
setIrqHandler:

                push    ds

                push    word ptr 0x0000
                pop     ds

                shl     bx, 2

                push    word ptr [ bx ]
                pop     cs:oldHandlerOff
                push    word ptr [ bx + 2 ]
                pop     cs:oldHandlerSeg

                pushf
                cli
                mov     word ptr [ bx ], ax
                mov     word ptr [ bx + 2 ], cx
                popf

                pop     ds
                ret

; end of setIrqHandler

oldHandler      label   dword
oldHandlerOff   dw      ?
oldHandlerSeg   dw      ?

                end
