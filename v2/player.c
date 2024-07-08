#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cards.h"
#include "player_tools.h"

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Please provide a CSV file or a binary file corresponding to a card set to review.\n");
		return 1;
	}

	// --help flag (may be the only flag --- cannot execute with --help and other flags)
	if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
	{
		print_help();
		return 0;
	}

	if(strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
	{
		print_version();
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
