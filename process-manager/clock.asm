                .model  tiny

                extern  setIrqHandler : near
                extern  oldHandler : dword

                public  C initClock

                .code

initClock:

                push    ax
                push    bx
                push    cx

                mov     bx, 0x0008
                mov     ax, offset clockIrqHandler
                mov     cx, cs
                call    setIrqHandler

                pop     cx
                pop     bx
                pop     ax
                ret

; end of initClock

clockIrqHandler:

                push    ax
                push    es

                mov     ax, 0xB800
                mov     es, ax

                mov     al, es:0x0001
                inc     al
                mov     es:0x0001, al

                pop     es
                pop     ax
                ;iret
                jmp     cs:oldHandler

; end of clockIrqHandler

                end
