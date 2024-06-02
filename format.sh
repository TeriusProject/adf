#!/bin/zsh
find src/ -iname '*.h' -o -iname '*.c' | xargs clang-format -i
find test/ -iname '*.h' -o -iname '*.c' | xargs clang-format -i
