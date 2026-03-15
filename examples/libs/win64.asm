global print
extern WriteFile
extern GetStdHandle
print:
push rbp
mov rbp, rsp
sub rsp, 40
mov rcx, -11
call GetStdHandle
mov rcx, rax
mov rdx, [rbp+24]
mov r8d, [rbp+16]
xor r9, r9
mov qword [rsp+32], 0
call WriteFile
add rsp, 40
pop rbp
ret
