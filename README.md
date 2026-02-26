# NeoQuark
The NeoQuark programming language
# Compile
compiler:
```
g++ nquark.cpp -o nquark
./nquark input.nqk -o output.asm
```
programm for linux:
```
nasm -f elf64 output.asm -o prog.o
ld prog.o -o prog
./prog
```
programm bin for own os:
```
nasm -f bin output.asm -o boot.bin
qemu-system-x86_64 boot.bin
```
