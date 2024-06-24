#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cards.h"

// Returns the amount of prompts answered correctly (incorrect amount can then be deduced)
unsigned short card_set_play(struct card_set *cs)
{
	// Setup for user input
	char *input_buffer = malloc(25);
	char input_c;
	size_t input_buffer_length = 0;
	size_t input_buffer_capacity = 25;

	// Statistics
	unsigned short correct_counter = 0;		// number of correctly answered
	unsigned short *correct_prompts = malloc(sizeof(unsigned short) * (cs->length)); // particular prompts answered correctly. Just assume the user gets them all right, then no extra allocation has to be done.
	if(correct_prompts == NULL)
	{
		fprintf(stderr, "Failed to allocate buffer to keep track of which prompts were answered correctly.\n");
		return -1;
	}
	
	for(unsigned short i = 0; i < cs->length; i++)
	{
		// Reset input buffer length
		input_buffer_length = 0;
		input_c = 1;

		// Prompt the user with the question
		printf("%s\n> ", cs->cards[i]->front);

		// get user input
		while(input_c != '\n')
		{
			// Get a character from stdin
			input_c = getc(stdin);

			// Make sure buffer is big enough to append it
			if(input_buffer_length >= input_buffer_capacity)
			{
				input_buffer_capacity *= 2;
				input_buffer = realloc(input_buffer, input_buffer_capacity);
				if(input_buffer == NULL)
				{
					fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
					return -1;
				}
			}
			if(input_c == '\n')
				continue;
			
			input_buffer[input_buffer_length] = input_c;
			input_buffer_length++;
		}
		input_buffer[input_buffer_length] = '\0';
		input_buffer_length++;

		// Validate answer
		if(card_validate_answer(cs->cards[i], input_buffer))
		{
			printf("[CORRECT!]\n");
			correct_prompts[correct_counter] = i;
			correct_counter++;
		}
		else
			printf("[INCORRECT!]\n");
	
	}
	// Review user's performance
	printf("\nRECAP:\nYou answered %hu prompts correctly!\n", correct_counter);
	printf("You answered the following prompts correctly: %hu", correct_prompts[0]);
	for(unsigned short i = 1; i < correct_counter; i++)
		printf(", %hu", i);

	printf("\n");

	// Cleanup
	free(input_buffer);
	free(correct_prompts);

	return correct_counter;
}

int main(void)
{
	struct card *card1 = card_create("What is your name?", "Dave", 0);
	card_print(card1);

	struct card *card2 = card_create("What are your favorite colors?", "orange", 3, "yellow", "brown", "green");
	card_print(card2);

	card_destroy(card1);
	card_destroy(card2);

	card1 = card_create("What is your name?", "Dave", 0);
	card2 = card_create("What are your favorite colors?", "orange", 3, "yellow", "brown", "green");
	struct card_set *cs = card_set_create("First set", "Random testing cards");
	if(cs == NULL)
	{
		fprintf(stderr, "Card set ended up NULL somehow.\n");
		card_destroy(card1);
		card_destroy(card2);
		return 1;
	}

	if(card_set_add_card(cs, card1))
		printf("Card added successfully.\n");
	else
		fprintf(stderr, "Failed to add card.\n");

	if(card_set_add_card(cs, card2))
		printf("Card added successfully.\n");
	else
		fprintf(stderr, "Failed to add card.\n");

	unsigned short correct = card_set_play(cs);
	if(correct >= cs->length * (3/4))
		printf("You did well!\n");
	else
		printf("You should practice some more.\n");

	card_set_print(cs);
	card_set_destroy(cs);
	return 0;
}
