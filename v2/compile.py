#!/usr/bin/env python3
from os import system
from sys import argv

def compile_cards():
    system("gcc -c cards.c -o cards.o")

def compile_player(): 
    system("gcc -c cards.c -o cards.o")
    system("gcc -c player_tools.c -o player_tools.o")
    system("gcc player.c cards.o player_tools.o -o player")

def compile_all():
    compile_player() # handles compilation of cards as well

argc: int = len(argv)
if len(argv) < 2:
    print("Please provide a file or set of files to compile. Alternatively you can enter  \"all\" as the only parameter to compile all files.\n")
    exit()

for file in argv[1:]:
    match file:
        case "cards" | "cards.c":
            compile_cards()

        case "player" | "player.c":
            compile_player()

        case "all" | "everything":
            compile_all()

        case _:
            print(f"Compilation procedure for \"{file}\" not defined.\n")
