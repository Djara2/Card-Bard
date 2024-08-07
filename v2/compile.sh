#!/bin/bash
gcc -c src/cards.c -o cards.o
gcc -c src/player_tools.c -o player_tools.o
ar rcs libcardboard.a cards.o player_tools.o
gcc player.c -L. -lcardboard -o player
