#!/bin/zsh
# Only works for macOS
MallocStackLogging=1 leaks  --atExit --outputGraph=$1  -- ./$1
