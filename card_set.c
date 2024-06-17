#include "card.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Creation methods
struct card_set* card_set_create(char *name)
{
	struct card_set *new_card_set = malloc(sizeof(struct card_set));
	if(new_card_set == NULL)
	{
		fprintf(stderr, "Failed to create new card set with name \"%s\". Instantiation returned a NULL pointer.\n", name); 
		return NULL;
	}

	new_card_set->name_length = strlen(name);
	new_card_set->name = malloc(sizeof(char) * (new_card_set->name_length + 1));

	// Default size of 10 cards
	new_card_set->cards = malloc(sizeof(struct card*) * 10);
	new_card_set->length = 0;
	new_card_set->capacity = 10;

	return new_card_set;
}

// Modification methods
bool card_set_add(struct card_set *set, struct card *c)
{
	// Check if the set has reached its max size
	if(set->length >= 32767)
	{
		fprintf(stderr, "Card set \"%s\" cannot be resized, because its card buffer has reached 32,767 cards.\n", set->name);
		return false;
	}

	// Allocate more space if necessary
	if(set->length >= set->capacity)
	{
		// Add space for 50 more cards
		unsigned short new_capacity = set->capacity + 50;
		if(new_capacity > 32767)
			new_capacity = 32767;

		// Realloc buffer
		set->capacity = new_capacity;
		set->cards = realloc(set->cards, sizeof(struct card*) * new_capacity);
		if(set->cards == NULL)
		{
			fprintf(stderr, "Failed to allocate more cards to the cards buffer of card set \"%s\". Realloc returned a NULL pointer. Capacity set to %hu with length %hu.\n", set->name, set->capacity, set->length);
			return false;
		}
	}

	// Add the card
	set->cards[set->length] = c;
	set->length++;

	return true;
}

bool card_set_remove_by_key(struct card_set *set, char *key)
{
	for(unsigned short i = 0; i < set->length; i++)
	{
		// Current card does not match
		if(strcmp(set->cards[i]->key, key) != 0)
			continue;

		// Card found
		card_destroy(set->cards[i]); // Destroy card

		// Shift upper half of set down (left)
		for(unsigned short j = i + 1; j < set->length; j++)
			set->cards[j-1] = set->cards[j];

		set->length--;
		return true;		
	}
	return false;
}

bool card_set_remove_by_index(struct card_set *set, unsigned short index)
{
	if(index < 0)
	{
		fprintf(stderr, "Cannot remove card at index %hu from card set \"%s\". The index must be 0 or greater.\n", index, set->name);
		return false;
	}

	if(index >= set->length)
	{
		fprintf(stderr, "Cannot remove card at index %hu from card set \"%s\". The index is too large (the length parameter of a card set is human readable, e.g. cannot remove at index 13 for a length 13 array).", index, set->name);
		return false;
	}

	// Destroy card
	card_destroy(set->cards[index]);

	// Shift upper part (after index) down (left)
	for(unsigned short i = index + 1; i < set->length; i++)
		set->cards[i-1] = set->cards[i];
	
	set->length--;
	return true;
}

// File IO
bool card_set_write_binary(struct card_set *set);

bool card_set_write_csv(struct card_set *set);

struct card_set* card_set_read_binary(char *file_name);

struct card_set* card_set_read_csv(char *file_name);

#endif
