                .model  tiny
                .186

                public  C InitClock

                extern  C HandleClockInterrupt : near
                extern  SetInterruptHandler : near
                extern  GetInterruptHandler : near
                extern  SaveContext : near
                extern  RestoreContext : near
                extern  InitKernelContext : near
                extern  LoadKernelContext : near

                .code

; Нет ввода
; нет вывода
InitClock:

                ; HACK: стек ядра должен храниться без текущего адреса возврата
                add     sp, 4
                call    InitKernelContext
                sub     sp, 4

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

                call    SaveContext
                mov     _processSs, ss
                mov     _processSp, sp

                call    LoadKernelContext

                push    offset _processSp
                push    offset _processSs
                call    HandleClockInterrupt

                mov     sp, _processSp
                mov     ss, _processSs
                call    RestoreContext

                jmp     [ cs:_pOldClockHandler ]

; конец _ClockHandler

_pOldClockHandler \
                dd      ?

_processSp      dw      ?
_processSs      dw      ?

                end
