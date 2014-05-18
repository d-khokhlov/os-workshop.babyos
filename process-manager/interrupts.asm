                .186
                .model  tiny

                public  SetInterruptHandler
                public  GetInterruptHandler

                .code

; Ввод:
;     bx - номер прерывания
;     cx:ax - адрес обрабокчика
; Нет вывода
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

; конец SetInterruptHandler

; Вввод:
;     bx - номер прерывания
; Вывод:
;     cx:ax - адрес обрабокчика
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

; конец GetInterruptHandler

                end
