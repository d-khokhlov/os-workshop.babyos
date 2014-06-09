                .model  tiny
                .186

                public  C InitClock

                extern  C HandleClockInterrupt : near
                extern  SetInterruptHandler : near
                extern  GetInterruptHandler : near
                extern  SaveContext : near
                extern  RestoreContext : near
                extern  LoadKernelContext : near

                .code

_TIMER_FREQUENCY_HZ \
                equ     1193180
_TIME_QUANTUM_MS \
                equ     1
_TIMER_COUNTER_START_VALUE \
                equ     _TIME_QUANTUM_MS * _TIMER_FREQUENCY_HZ / 1000

;_pOldClockHandler \
;                dd      ?

_processSp      dw      ?
_processSs      dw      ?

; void public InitClock();
InitClock:
                push    bx
                push    es
                push    di

                mov     al, 00110110b
                out     0x43, al
                mov     al, _TIMER_COUNTER_START_VALUE and 0xFF
                out     0x40, al
                mov     al, _TIMER_COUNTER_START_VALUE shr 8
                out     0x40, al

                mov     bx, 0x0008

                ;call    GetInterruptHandler
                ;mov     word ptr _pOldClockHandler, di
                ;mov     word ptr _pOldClockHandler + 2, es

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

                mov     [ cs:_processSs ], ss
                mov     [ cs:_processSp ], sp

                call    LoadKernelContext

                push    offset _processSp
                push    offset _processSs
                call    HandleClockInterrupt
                add     sp, 4

                mov     ss, _processSs
                mov     sp, _processSp

                mov     al, 0x20
                out     0x20, al

                call    RestoreContext

                iret
                ;jmp     [ cs:_pOldClockHandler ]
; конец _ClockHandler

                end
