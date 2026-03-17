global printch
global printstr
global readch
global cls
global exit
global set_320x200
global set_80x25
global put_pixel
printch:
mov ah, 0x0e
int 0x10
ret
printstr:
push bp
mov bp, sp
mov si, [bp+4]
.loop:
lodsb
cmp al, 0
je .done
mov ah, 0x0e
int 0x10
jmp .loop
.done:
pop bp
ret
cls:
mov ax, 0x0003
int 0x10
ret
readch:
mov ah, 0x00
int 0x16
ret
set_320x200:
mov ax, 0x0013
int 0x10
ret
set_80x25:
mov ax, 0x0003
int 0x10
ret
put_pixel:
push bp
mov bp, sp
push ax
push cx
push dx
push di
push es
mov cx, [bp+8]
mov dx, [bp+6]
mov ax, [bp+4]
push ax
mov ax, 0xa000
mov es, ax
pop ax
push ax
mov ax, dx
mov dx, 320
mul dx, 
add ax, cx
mov di, ax
pop ax
stosb
pop es
pop di
pop dx
pop cx
pop ax
pop bp
ret
exit:
jmp $
ret
BLACK equ  0x00
BLUE equ  0x01
GREEN equ  0x02
CYAN equ  0x03
RED equ  0x04
MAGENTA equ  0x05
BROWN equ  0x06
LIGHT_GRAY equ  0x07
DARK_GRAY equ  0x08
LIGHT_BLUE equ  0x09
LIGHT_GREEN equ  0x0A
LIGHT_CYAN equ  0x0B
LIGHT_RED equ  0x0C
LIGHT_MAGENTA equ  0x0D
YELLOW equ  0x0E
WHITE equ  0x0F
buffer:
times 64 db 0
