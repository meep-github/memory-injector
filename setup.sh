#!/bin/bash

OUTNAME=inject
OUTNAME2=hello
SOURCE1="src/inject.c"
SOURCE2="src/hello.c"

gcc -o ./$OUTNAME $SOURCE1
gcc -o ./$OUTNAME2 $SOURCE2

