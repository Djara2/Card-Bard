#!/bin/bash
if [ "$1" = "cards" ]; then
	gcc -c cards.c -o cards.o
else
	gcc "$1.c" cards.o -o "$1"
fi
