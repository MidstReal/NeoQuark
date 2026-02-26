global print
extern WriteFile
extern GetStdHandle
print:
sub  rsp, 40
mov  rcx, -11
call GetStdHandle
mov  rcx, rax
xor  r9, r9
mov  qword [rsp + 32], 0
call WriteFile
add  rsp, 40
ret
