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

;_pOldClockHandler \
;                dd      ?

_processSp      dw      ?
_processSs      dw      ?

; void public InitClock();
InitClock:
                push    bx
                push    es
                push    di

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

                ; DEBUG
                ;mov     ax, 0xB800
                ;mov     ds, ax
                ;mov     bx, 80 * 2 * 20
                ;mov     al, [ bx ]
                ;inc     al
                ;mov     [ bx ], al
                ; END DEBUG

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
