#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cards.h"

enum MODES {REGULAR, REGULAR_RANDOM, BACK_FIRST, BACK_FIRST_RANDOM};

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

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Please provide a CSV file or a binary file corresponding to a card set to review.\n");
		return 1;
	}

	// --help flag (may be the only flag --- cannot execute with --help and other flags)
	if(strcmp(argv[1], "--help") == 0 | strcmp(argv[1], "-h") == 0)
	{
		print_help();
		return 0;
	}

	/* first bit controls back first toggle
	   second bit controls random toggle
	   bits[0] | bits[1] | MODE 
	   0       | 0       | regular
	   0       | 1       | regular random
	   1       | 0       | back first
	   1       | 1       | back first random */
	byte mode_bits[2] = {0, 0};

	// Read through flags and set mode based on that
	enum MODES mode = REGULAR;
	byte input_index = parse_flags(mode_bits, argc, argv);
	set_mode(&mode, mode_bits);

	// Read card set provided --- INPUT MUST ALWAYS BE THE FIRST ARGUMENT
	struct card_set *cs = card_set_create_from_csv(argv[input_index]);
	if(cs == NULL)
	{
		fprintf(stderr, "Failed to read the card set provided into memory.\n");
		return 1;
	}

	unsigned short correct;
	switch(mode)
	{
		case REGULAR:
			printf("Playing in regular mode.\n");
			correct = card_set_play(cs); 
			break;

		case REGULAR_RANDOM:
			printf("Playing in regular random mode.\n");
			correct = card_set_play_random(cs);
			break;
		
		case BACK_FIRST:
			printf("Back first mode unimplemented.\n");
			return 0;
			break;

		case BACK_FIRST_RANDOM:
			printf("Back first random mode unimplemented.\n");
			return 0;
			break;

		default:
			correct = card_set_play(cs);
			break;
	}

	if(correct >= ((3/4) * cs->length)) 
		printf("You did well!\n");

	free(cs);
	return 0;
}
