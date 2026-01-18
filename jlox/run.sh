#!/bin/zsh
set -e

if ! [ -z "$0" ]; then
    java -classpath ./bin lox.Lox "$0"
fi