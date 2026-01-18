#!/bin/zsh
set -e

if command -v clang > /dev/null; then
    clang -o main *.c
else
    gcc -o main *.c
fi