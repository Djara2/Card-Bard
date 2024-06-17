#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "card.h"

struct card_entry* card_entry_create(char *key, char *value)
{
	struct card_entry *new_card_entry = malloc(sizeof(struct card_entry));
	if(new_card_entry == NULL)
	{
		fprintf(stderr, "Failed to create new card_entry with key \"%s\" and value \"%s\".\n", key, value);
		return NULL;
	}

	new_card_entry->key = (char*) malloc(strlen(key) + 1);
	new_card_entry->values = malloc(sizeof(char*) * 1);
	new_card_entry->values[0] = (char*) malloc(strlen(value) + 1);
	new_card_entry->values_length = 1;

	strcpy(new_card_entry->key, key);
	strcpy(new_card_entry->values[0], value);

	new_card_entry->next = NULL;

	return new_card_entry;
}

struct card_entry* card_entry_create_many_answers(char *key, char **values, unsigned char length)
{
	struct card_entry *new_card_entry = malloc(sizeof(struct card_entry));
	if(new_card_entry == NULL)
	{
		fprintf(stderr, "Failed to create new card_entry with key \"%s\" corresponding to %c values.\n", key, length);
		return NULL;
	}

	new_card_entry->key = (char*) malloc(strlen(key) + 1);
	new_card_entry->values = malloc(sizeof(char*) * length);
	for(unsigned char i = 0; i < length; i++)
	{
		new_card_entry->values[i] = (char*) malloc(strlen(values[i]) + 1);
		strcpy(new_card_entry->values[i], values[i]);
	}

	new_card_entry->values_length = length;
	strcpy(new_card_entry->key, key);

	new_card_entry->next = NULL;

	return new_card_entry;
}

bool card_entry_add_value(struct card_entry *card, char *value)
{
	if(card->values_length >= 255)
	{
		fprintf(stderr, "Cannot add more elements to the values buffer of the card \"%s\". The length reached 255 values.\n", value);
		return false;
	}

	card->values = realloc(card->values, (card->values_length + 1) * sizeof(char*));
	if(card->values == NULL)
	{
		fprintf(stderr, "Failed to allocate more memory for values buffer of card with key \"%s\".\n", card->key);
		return false;
	}

}

// SUPPOSES THAT THERE IS NO LINEAR CHAINING
void card_entry_destroy(struct card_entry *e)
{
	free(e->key);
	for(unsigned char i = 0; i < e->values_length; i++)
		free(e->values[i]);

	free(e->values);
	free(e);
	return;
}
