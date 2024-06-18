#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "card.h"
#include "card_set.h"

int main(int argc, char **argv)
{
	struct card_set *set = card_set_create("Test set");

	struct card *card1 = card_create("What color is the sky", "blue");

	struct card *card2 = card_create("What color is a cat", "orange");
	card_add_value(card2, "black");
	card_add_value(card2, "grey");
	card_add_value(card2, "white");

	card_set_add(set, card1);
	card_set_add(set, card2);

	card_set_print(set);

	card_destroy(card1);
	card_destroy(card2);
	free(set->cards);
	free(set->name);
	free(set);

	return 0;
}
