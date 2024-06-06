#!/bin/zsh
emcc ../../src/adf.c -o adflib.js -sMODULARIZE -sEXPORTED_RUNTIME_METHODS=ccall -std=c17  
