# NeoQuark
NeoQuark is a programming language designed to simplify working with the NASM assembler.
# Compile
compiler:
```
bash bd.sh
```
program for linux:
```
./nquark input.nqk -o output.asm -64

nasm -f elf64 output.asm -o prog.o
ld prog.o -o prog

./prog
```
program bin for own 16-bit os:
```
./nquark input.nqk -o output.asm -16
nasm -f bin output.asm -o file.bin
```
