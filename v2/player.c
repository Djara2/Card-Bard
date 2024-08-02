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

	// Create buffer for accepting user input for PLAYER APPLICATION
	// (functions from cards.h will create their own buffers)
	char *player_input_buffer = malloc(sizeof(char) * 25);
	if(player_input_buffer == NULL)
	{
		fprintf(stderr, "Failed to allocate initial 25 bytes for player application input buffer. Note taht this buffer is different from the one used by the card_set_play function in cards.h).\n");
		card_set_destroy(cs);
		return 1;
	}
	size_t player_input_buffer_length = 0;
	size_t player_input_buffer_capacity = 25;

	// Create buffer for tracking prompts answered incorrectly.
	unsigned short incorrect_count;
	unsigned short *incorrect_prompts = malloc(sizeof(unsigned short) * cs->length);
	if(incorrect_prompts == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for buffer keeping track of prompts answered incorrectly.\n");
		card_set_destroy(cs);
		free(player_input_buffer);
		return 1;
	}

	// Data for continuously prompting user to retry on prompts they
	// answered incorrectly
	bool continue_looping = true;
 
	switch(mode)
	{
		case REGULAR:
			printf("Playing in regular mode.\n");
			incorrect_count = card_set_play(cs, incorrect_prompts);
			if(incorrect_count == 0)
				break;

			// Prompt user to revisit the prompts they answered incorrectly
			printf("You answered %hu prompts incorrectly. Revisit those prompts?\n> ", incorrect_count);
			get_input(&player_input_buffer, &player_input_buffer_length, &player_input_buffer_capacity);
			if(player_input_buffer[0] == 'n')
				break;
			
			// Start a new playthrough, reviewing only	
			while(continue_looping)
			{
				incorrect_count = card_set_play_indices(cs, incorrect_prompts, incorrect_count, incorrect_prompts);
				// Prompt user to revisit the prompts they answered incorrectly
				printf("You answered %hu prompts incorrectly. Revisit those prompts?\n> ", incorrect_count);
				get_input(&player_input_buffer, &player_input_buffer_length, &player_input_buffer_capacity);
				if(player_input_buffer[0] == 'n')
					continue_looping = false;

				printf("[DEBUGGING] accepted user input.\n");
			}

			// Go to exit application procedure.
			break;

		case REGULAR_RANDOM:
			printf("Playing in regular random mode.\n");
			incorrect_count = card_set_play_random(cs);
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
			incorrect_count = card_set_play(cs, incorrect_prompts);
			break;
	}

	if(incorrect_count < ((3/4) * cs->length)) 
		printf("You did well!\n");

	card_set_destroy(cs);
	free(player_input_buffer);
	return 0;
}
