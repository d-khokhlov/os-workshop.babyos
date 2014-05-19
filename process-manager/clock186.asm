                .model  tiny
                .186

                public  C InitClock

                extern  SetInterruptHandler : near
                extern  GetInterruptHandler : near

                .code

; Нет ввода
; нет вывода
InitClock:

                push    bx
                push    es
                push    di

                mov     bx, 0x0008

                call    GetInterruptHandler
                mov     word ptr _pOldClockHandler, di
                mov     word ptr _pOldClockHandler + 2, es

                mov     di, cs
                mov     es, di
                mov     di, offset _ClockHandler
                call    SetInterruptHandler

                pop     di
                pop     es
                pop     bx
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
