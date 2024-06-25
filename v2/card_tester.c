#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cards.h"
#define byte unsigned char

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

	struct card_set *cs2 = card_set_create_from_csv("csv_set.csv");
	if(cs2 != NULL)
	{
		correct = card_set_play(cs2);
		if(correct >= cs->length * (3/4))
			printf("You did well!\n");
		else
			printf("You should practice some more.\n");
	}
	else printf("card set from CSV ended up NULL.\n");
	card_set_print(cs);
	card_set_print(cs2);
	card_set_destroy(cs);
	card_set_destroy(cs2);

	printf("tokens test:\n");
	char **tokens = malloc(sizeof(char*) * 5);
	byte tokens_length = 0;
	byte tokens_capacity = 5;
	char *str = malloc(strlen("Dave,Mayo,Infected Mushroom,Pig Destroyer") + 1);
	strcpy(str, "Dave,Mayo,Infected Mushroom,Pig Destroyer");
	get_tokens(str, ",", &tokens, &tokens_length, &tokens_capacity);
	for(int i = 0; i < tokens_length; i++)
	{
		printf("%s\n", tokens[i]);
		free(tokens[i]);
	}
	free(tokens);
	return 0;
}
