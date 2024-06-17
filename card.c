#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "card.h"

struct card* card_create(char *key, char *value)
{
	struct card *new_card = malloc(sizeof(struct card));
	if(new_card == NULL)
	{
		fprintf(stderr, "Failed to create new card with key \"%s\" and value \"%s\".\n", key, value);
		return NULL;
	}

	new_card->key = (char*) malloc(strlen(key) + 1);
	new_card->values = malloc(sizeof(char*) * 1);
	new_card->values[0] = (char*) malloc(strlen(value) + 1);
	new_card->values_length = 1;

	strcpy(new_card->key, key);
	strcpy(new_card->values[0], value);

	return new_card;
}

struct card* card_create_many_answers(char *key, char **values, unsigned char length)
{
	struct card *new_card = malloc(sizeof(struct card));
	if(new_card == NULL)
	{
		fprintf(stderr, "Failed to create new card with key \"%s\" corresponding to %c values.\n", key, length);
		return NULL;
	}

	new_card->key = (char*) malloc(strlen(key) + 1);
	new_card->values = malloc(sizeof(char*) * length);
	for(unsigned char i = 0; i < length; i++)
	{
		new_card->values[i] = (char*) malloc(strlen(values[i]) + 1);
		strcpy(new_card->values[i], values[i]);
	}

	new_card->values_length = length;
	strcpy(new_card->key, key);

	return new_card;
}

bool card_add_value(struct card *card, char *value)
{
	// Case 1: too many values
	if(card->values_length >= 255)
	{
		fprintf(stderr, "Cannot add more elements to the values buffer of the card \"%s\". The length reached 255 values.\n", value);
		return false;
	}

	// Add value
	card->values = realloc(card->values, (card->values_length + 1) * sizeof(char*));
	if(card->values == NULL)
	{
		fprintf(stderr, "Failed to allocate more memory for values buffer of card with key \"%s\".\n", card->key);
		return false;
	}

	card->values[card->values_length] = malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(card->values[card->values_length], value);
	return true;
}

void card_destroy(struct card *e)
{
	free(e->key);
	for(unsigned char i = 0; i < e->values_length; i++)
		free(e->values[i]);

	free(e->values);
	free(e);
	return;
}
