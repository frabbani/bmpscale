#!/bin/bash

echo "compiling..."
g++ -std=c++17 -c "main.cpp"
gcc -std=c99 -c "bitmap\\bitmap.c"
gcc -std=c99 -c "bitmap\\defs.c"
g++ -o prog.exe main.o bitmap.o defs.o
echo "done!"