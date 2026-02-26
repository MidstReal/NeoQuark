# NeoQuark
The NeoQuark programming language
# Compile
compiler:
```
g++ nquark.cpp -o nquark
./nquark input.nqk -o output.asm
```
program for linux:
```
./nquark input.nqk -o output.asm

nasm -f elf64 output.asm -o prog.o
ld prog.o -o prog

./prog
```
program for windows:
```
./nquark.exe input.nqk -o output.asm
nasm -f win64 output.asm -o prog.o
```
```
ld prog.o -o prog.exe
```
if you use libs/win.asm:
```
ld prog.o -o prog.exe -l kernel32
```
```
./prog.exe
```
program bin for own os:
```
nasm -f bin output.asm -o boot.bin
qemu-system-x86_64 boot.bin
```
