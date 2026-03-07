global printch
global printstr
global readch
global cls
global exit
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
buffer  times 64 db 0
