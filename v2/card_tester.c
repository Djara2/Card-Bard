#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cards.h"

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

	card_set_print(cs);
	card_set_destroy(cs);
	return 0;
}
