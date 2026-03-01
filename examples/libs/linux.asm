global print
global read
global exit
global strlen
print:
mov rax, 1
mov rdi, 1
syscall
ret
read:
mov rdx, rsi
mov rsi, rdi
mov rdi, 0
mov rax, 0
syscall
ret
exit:
mov rax, 60
syscall
ret
strlen:
xor rax, rax
.loop:
cmp byte [rdi+rax], 0
je .done
inc rax
jmp .loop
.done:
ret
