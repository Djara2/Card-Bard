#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cards.h"

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Please provide a CSV file or a binary file corresponding to a card set to review.\n");
		return 1;
	}

	struct card_set *cs = card_set_create_from_csv(argv[1]);
	if(cs == NULL)
	{
		fprintf(stderr, "Failed to read the card set provided into memory.\n");
		return 1;
	}

	unsigned short correct = card_set_play(cs);
	if(correct >= ((3/4) * cs->length))
		printf("You did well!\n");

	free(cs);
	return 0;
}
