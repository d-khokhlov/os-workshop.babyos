                .model  tiny
                .186

                public  SetInterruptHandler
                public  GetInterruptHandler

                .code

; Ввод:
;     bx - номер прерывания
;     es:di - адрес обрабокчика
; Нет вывода
SetInterruptHandler:
                push    ds

                push    word ptr 0x0000
                pop     ds

                shl     bx, 2

                pushf
                cli
                mov     word ptr [ bx ], di
                mov     word ptr [ bx + 2 ], es
                popf

                shr     bx, 2

                pop     ds
                ret
; конец SetInterruptHandler

; Вввод:
;     bx - номер прерывания
; Вывод:
;     es:di - адрес обрабокчика
GetInterruptHandler:
                push    ds

                push    word ptr 0x0000
                pop     ds

                shl     bx, 2

                mov     di, word ptr [ bx ]
                mov     es, word ptr [ bx + 2 ]

                shr     bx, 2

                pop     ds
                ret
; конец GetInterruptHandler

                end
