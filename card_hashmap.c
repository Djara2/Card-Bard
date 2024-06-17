#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card_hashmap.h"
#include "card.h"
#include <stdbool.h>

unsigned int hash_string(char *input)
{
	unsigned int sum = 0;
	for(int i = 0; i < strlen(input); i++)
		sum += input[i];

	unsigned int hash_value = (SCALAR * sum) % MODULUS;
	return hash_value;
}

struct hashmap* hashmap_create()
{
	struct hashmap *new_hashmap = malloc(sizeof(struct hashmap));
	if(new_hashmap == NULL)
	{
		fprintf(stderr, "Failed to allocate sufficient memory for a new hashmap.\n");
		return NULL;
	}

	new_hashmap->entries = malloc(sizeof(struct card_entry*) * MODULUS);
	if(new_hashmap->entries == NULL)
	{
		fprintf(stderr, "Failed to allocate sufficient memory for %d card_entry pointers when initializing the new hashmap's entries array. The hashmap's memory will be freed and NULL will be returned.\n", MODULUS);
		free(new_hashmap);
		return NULL;
	}

	new_hashmap->length = 0;

	return new_hashmap;
}

bool hashmap_insert(struct hashmap *map, char *key, char *value)
{
	unsigned int index = hash_string(key);

	// Case 1: index is unoccupied
	if(map->entries[index] == NULL)
	{
		map->entries[index] = card_entry_create(key, value);
		return true;
	}
	// Case 2: index is already occupied --- do linear probing
	// Linear probing
	struct card_entry *current = malloc(sizeof(struct card_entry*));
	current = map->entries[index];
	while(current->next != NULL)
	{
		// Subcase 1: The item has already been inserted --- stop probing
		if(strcmp(current->key, key) == 0)
			return false;

		current = current->next; 
	}

	// Insert new card_entry and increase length
	current->next = card_entry_create(key, value);
	map->length++;

	return true;
}

bool hashmap_insert_key_and_array(struct hashmap *map, char *key, char **values, unsigned char length)
{
	unsigned int index = hash_string(key);

	// Case 1: index is unoccupied
	if(map->entries[index] == NULL)
	{
		map->entries[index] = card_entry_create_many_answers(key, values, length);
		return true;
	}
	// Case 2: index is already occupied --- do linear probing
	// Linear probing
	struct card_entry *current = malloc(sizeof(struct card_entry*));
	current = map->entries[index];
	while(current->next != NULL)
	{
		// Subcase 1: The item has already been inserted --- stop probing
		if(strcmp(current->key, key) == 0)
			return false;

		current = current->next; 
	}

	// Insert new card_entry and increase length
	current->next = card_entry_create_many_answers(key, values, length);
	map->length++;

	return true;
}

bool hashmap_contains(struct hashmap *map, char *key)
{
	unsigned int index = hash_string(key);

	// Case 1: index is unoccupied
	if(map->entries[index] == NULL)
		return false;

	// Case 2: index is already occupied --- do linear probing
	// Linear probing
	struct card_entry *current = malloc(sizeof(struct card_entry*));
	current = map->entries[index];
	while(current != NULL)
	{
		// Subcase 1: The item has already been inserted --- stop probing
		if(strcmp(current->key, key) == 0)
			return true;

		current = current->next; 
	}

	return false;
}

bool hashmap_remove(struct hashmap *map, char *key)
{
	unsigned int index = hash_string(key);

	// Case 1: index is unoccupied
	if(map->entries[index] == NULL)
		return false; // cannot remove something inexistent

	// Case 2: index is already occupied --- do linear probing
	// Linear probing
	struct card_entry *current = map->entries[index];
	struct card_entry *previous = NULL; 
	while(current != NULL)
	{
		// Element found
		if(strcmp(current->key, key) == 0)
		{
			// Update links 
			// Case 1: If the first node in the chain is the one
			//         to be deleted
			if(previous == NULL)
				map->entries[index] = current->next;

			// Case 2: If the node is part of a series.
			else
				previous->next = current->next;

			// Destroy current node
			free(current->key);
			for(unsigned char i = 0; i < current->values_length; i++)
				free(current->values[i]);

			free(current->values);	
			free(current);

			return true;
		}

		previous = current;
		current = current->next; 
	}

	return false;
}

void hashmap_destroy(struct hashmap *map)
{
	// go through every single card_entry and clear them all if they are not NULL
	// as well as all of the chaining
	for(int i = 0; i < MODULUS; i++)
	{
		if(map->entries[i] == NULL)
			continue;

		// remove entire chain
		struct card_entry *current = map->entries[i];
		while(current != NULL)
		{
			free(current->key);
			for(unsigned char j = 0; j < current->values_length; j++)
				free(current->values[j]);

			free(current->values);
			struct card_entry *previous = current;
			current = current->next;
			free(previous);
		}
	}

	free(map->entries);
	free(map);
	return;
}
