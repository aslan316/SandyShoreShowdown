#!/bin/bash
gcc *.c -L ../raylib/src/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -I ../raylib/src/  -o gameTest
