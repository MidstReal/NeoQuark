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
programm for windows:
```
nquark.exe input.nqk -o output.asm -64

nasm.exe -f win64 output.asm -o prog.obj
golink.exe prog.obj kernel32.dll /console /entry:_start

prog.exe
```
nasm:   https://www.nasm.us/pub/nasm/releasebuilds/
golink: http://www.godevtool.com/Golink.zip

program bin for own 16-bit os:
```
./nquark input.nqk -o output.asm -16
nasm -f bin output.asm -o file.bin
```


parse with comments:
```
[compile command] -dbg

./nquark input.nqk -o output.asm -64 -dbg
```
