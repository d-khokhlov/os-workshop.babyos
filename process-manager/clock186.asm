                .model  tiny
                .186

                public  C InitClock

                extern  C HandleClockInterrupt : near
                extern  SetInterruptHandler : near
                extern  GetInterruptHandler : near
                extern  SaveContext : near
                extern  RestoreContext : near
                extern  LoadKernelContext : near

                extern  DEBUG_PrintState : near

                .code

_pOldClockHandler \
                dd      ?

_processSp      dw      ?
_processSs      dw      ?

_isClockHandled dw      0

; void _cdecl InitClock();
InitClock:
                call    DEBUG_PrintState
                ret

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
                ; DEBUG
                cmp     word ptr cs:_isClockHandled, 1
                je      @callOldHandler
                mov     word ptr cs:_isClockHandled, 1
                ; END DEBUG

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
                call    RestoreContext

@callOldHandler: ; DEBUG LABEL
                jmp     [ cs:_pOldClockHandler ]
; конец _ClockHandler

                end
