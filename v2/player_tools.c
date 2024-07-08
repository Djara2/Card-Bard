#include "player_tools.h"
#include "cards.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Returns the index at which the input file is found
byte parse_flags(byte *mode_bits, int argc, char **argv)
{
	byte input_index = 1;
	byte i = 1;
	while(i < argc)
	{
		byte token_length = strlen(argv[i]);

		// Shortened flag found (perhaps combined)
		if(argv[i][0] == '-' && argv[i][1] != '-')
		{
			// First check for input flag, which cannot be combined with others
			if(argv[i][1] == 'i')
			{
				// --input flag provided, but file does not follow it.
				// check for -1 in caller and abort
				if(i + 1 >= argc)
					input_index = -1;

				// --input flag provided and next token will be taken as the return index
				// for where to look in argv to grab the input file
				input_index = i + 1;
				i++;		// can skip over next token since it MUST be the input file
				continue;
			}

			// Grab each individual flag (e.g. -rb means random and back sides first)
			for(byte j = 1; j < token_length; j++)
			{
				switch(argv[i][j])
				{
					case 'n':	// normal (regular)
						mode_bits[0] = 0;
						break;

					case 'b':	// back first
						mode_bits[0] = 1;
						break;

					case 'r':	// random
						mode_bits[1] = 1;
						break;

					default:
						break;
				}
			}
		}

		// Verbose flag found
		else if(argv[i][0] == '-' && argv[i][1] == '-')
		{
			if(strcmp(argv[i], "--normal") == 0 || strcmp(argv[i], "--regular") == 0)
				mode_bits[0] = 0;

			if(strcmp(argv[i], "--backfirst") == 0 || strcmp(argv[i], "--back") == 0)
				mode_bits[0] = 1;

			if(strcmp(argv[i], "--random") == 0)
				mode_bits[1] = 1;
			
			if(strcmp(argv[i], "--input") == 0)
			{
				// --input flag provided, but file does not follow it.
				// check for -1 in caller and abort
				if(i + 1 >= argc)
					input_index = -1;

				// --input flag provided and next token will be taken as the return index
				// for where to look in argv to grab the input file
				input_index = i + 1;
				i++;		// can skip over next token since it MUST be the input file
			}
		}
		// Treat argument as the input file
		else
			input_index = i;

		i++;		// continue looping

	}

	return input_index;
}

void set_mode(enum MODES *m, byte *bits)
{
	if(bits[0] == 0)
	{
		if(bits[1] == 0)
			(*m) = REGULAR;

		if(bits[1] == 1)
			(*m) = REGULAR_RANDOM;
	}
	else
	{
		if(bits[1] == 0)
			(*m) = BACK_FIRST;
		
		if(bits[1] == 1)
			(*m) = BACK_FIRST_RANDOM;
	}
}

void print_help(void)
{
	FILE *help_file = fopen("help.txt", "r");
	if(help_file == NULL)
	{
		fprintf(stderr, "Help file cannot be opened. Does it exist at the expected path?\n");
		return;
	}

	char c = 0;
	while(c != EOF)
	{
		c = fgetc(help_file);
		printf("%c", c);
	}
	fclose(help_file);
}

void print_version(void)
{
	FILE *version_file = fopen("version.txt", "r");
	if(version_file == NULL)
	{
		fprintf(stderr, "Version file cannot be opened. Does it exist at the expected path?\n");
		return;
	}

	char c = 0;
	while(c != EOF)
	{
		c = fgetc(version_file);
		printf("%c", c);
	}
	fclose(version_file);
}
