                .model  tiny

                .code

_Start:
                mov     bp, sp
                mov     ax, [ bp + 4 ]

                mov     bx, 160
                mul     bx

                mov     bx, ax
                add     bx, 160

                mov     cx, 0xB800
                mov     es, cx

                mov     dx, 0

                mov     cx, 0xFFFF
@beginLine:

                mov     di, ax

@printChar:
                mov     byte ptr es:di, dl
                inc     dl
                inc     di
                mov     byte ptr es:di, dh
                inc     dh
                and     dh, 0x0F
                inc     di

                cmp     di, bx
                jb      @printChar

                loop    @beginLine

                retf

                end     _Start
