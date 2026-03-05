# NeoQuark
The NeoQuark programming language
# Compile
compiler:
```
g++ nquark.cpp -o nquark
```
program for linux:
```
./nquark input.nqk -o output.asm 64bits

nasm -f elf64 output.asm -o prog.o
ld prog.o -o prog

./prog
```
program bin for own 16-bit os:
```
./nquark input.nqk -o output.asm 16bits
nasm -f bin output.asm -o file.bin
```
program bin for own 32-bit os:
```
./nquark input.nqk -o output.asm 32bits
nasm -f bin output.asm -o file.bin
```
