global printch
global printstr
global readch
global cls
global exit
global strcmp
global chkcmd
printch:
mov ah, 0x0e
int 0x10
ret
printstr:
lodsb
cmp al, 0
je .done
mov ah, 0x0e
int 0x10
jmp printstr
.done:
ret
cls:
mov ax, 0x0003
int 0x10
ret
readch:
mov ah, 0x00
int 0x16
ret
exit:
jmp $
ret
strcmp:
.loop:
mov al, [si]
mov bl, [di]
cmp al, bl
jne .not_equal
cmp al, 0
je .equal
inc si
inc di
jmp .loop
.not_equal:
clc
ret
.equal:
stc
ret
chkcmd:
mov si, endl
call printstr
mov si, buffer
call strcmp
ret
buffer  times 64 db 0
