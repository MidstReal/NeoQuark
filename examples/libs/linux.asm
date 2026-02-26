global print
global exit
print:
mov rax, 1
mov rdi, 1
syscall
ret
exit:
mov rax, 60
syscall
ret
