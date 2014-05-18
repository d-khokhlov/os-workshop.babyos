                .model  tiny

                public  C InitClock

                extern  SetInterruptHandler : near
                extern  GetInterruptHandler : near

                .code

; Нет ввода
; нет вывода
InitClock:

                push    ax
                push    bx
                push    cx

                mov     bx, 0x0008

                call    GetInterruptHandler
                mov     word ptr _pOldClockHandler, ax
                mov     word ptr _pOldClockHandler + 2, cx

                mov     cx, cs
                mov     ax, offset _ClockHandler
                call    SetInterruptHandler

                pop     cx
                pop     bx
                pop     ax
                ret

; конец InitClock

; Обработчик прерывания
_ClockHandler:

                push    ax
                push    es

                mov     ax, 0xB800
                mov     es, ax

                mov     al, es:0x0001
                inc     al
                mov     es:0x0001, al

                pop     es
                pop     ax

                jmp     [ cs:_pOldClockHandler ]

; конец _ClockHandler

_pOldClockHandler \
                dd      ?

                end
