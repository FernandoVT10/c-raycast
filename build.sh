#!/bin/bash

FILES="main.c"
gcc -Wall -Werror -o main $FILES -I./raylib-5.5/include -L./raylib-5.5/lib/ -l:libraylib.a -lm
