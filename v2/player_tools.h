#include "cards.h"

#ifndef PLAYER_TOOLS_H
#define PLAYER_TOOLS_H
enum MODES {REGULAR, REGULAR_RANDOM, BACK_FIRST, BACK_FIRST_RANDOM};

// Returns the index at which the input file is found
byte parse_flags(byte *mode_bits, int argc, char **argv);

void set_mode(enum MODES *m, byte *bits);

void print_help(void);

void print_version(void);

#endif
