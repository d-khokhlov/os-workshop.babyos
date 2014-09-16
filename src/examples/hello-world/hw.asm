                .model  tiny
                .186

                .code

_Start:
                mov     ah, 0x09
                mov     dx, offset _message
                int     0x21

                mov     di, 0
                mov     ax, offset _executablePath
                mov     dx, 21
                int     0x90

                mov     di, 0
                mov     ax, offset _path
                mov     dx, 0
                int     0x90

                mov     di, 1
                int     0x90

_message        db      "Hello, World!", 0x0Dh, 0x0A, '$'
_executablePath db      "randstr", 0
_path           db      "worm", 0

                end     _Start
