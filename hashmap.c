#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include <stdbool.h>

unsigned int hash_string(char *input)
{
	unsigned int sum = 0;
	for(int i = 0; i < strlen(input); i++)
		sum += input[i];

	unsigned int hash_value = (SCALAR * sum) % MODULUS;
	return hash_value;
}

struct entry* entry_create(char *key, char *value)
{
	struct entry *new_entry = malloc(sizeof(struct entry));
	if(new_entry == NULL)
	{
		fprintf(stderr, "Failed to create new entry with key \"%s\" and value \"%s\".\n", key, value);
		return NULL;
	}

	new_entry->key = (char*) malloc(strlen(key) + 1);
	new_entry->value = (char*) malloc(strlen(value) + 1);
	strcpy(new_entry->key, key);
	strcpy(new_entry->value, value);

	new_entry->next = NULL;

	return new_entry;
}

// SUPPOSES THAT THERE IS NO LINEAR CHAINING
void entry_destroy(struct entry *e)
{
	free(e->key);
	free(e->value);
	free(e);
	return;
}

struct hashmap* hashmap_create()
{
	struct hashmap *new_hashmap = malloc(sizeof(struct hashmap));
	if(new_hashmap == NULL)
	{
		fprintf(stderr, "Failed to allocate sufficient memory for a new hashmap.\n");
		return NULL;
	}

	new_hashmap->entries = malloc(sizeof(struct entry*) * MODULUS);
	if(new_hashmap->entries == NULL)
	{
		fprintf(stderr, "Failed to allocate sufficient memory for %d entry pointers when initializing the new hashmap's entries array. The hashmap's memory will be freed and NULL will be returned.\n", MODULUS);
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
		map->entries[index] = entry_create(key, value);
		return true;
	}
	// Case 2: index is already occupied --- do linear probing
	// Linear probing
	struct entry *current = malloc(sizeof(struct entry*));
	current = map->entries[index];
	while(current->next != NULL)
	{
		// Subcase 1: The item has already been inserted --- stop probing
		if(strcmp(current->key, key) == 0)
			return false;

		current = current->next; 
	}

	// Insert new entry and increase length
	current->next = entry_create(key, value);
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
	struct entry *current = malloc(sizeof(struct entry*));
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
	struct entry *current = map->entries[index];
	struct entry *previous = NULL; 
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
			free(current->value);
			
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
	// go through every single entry and clear them all if they are not NULL
	// as well as all of the chaining
	for(int i = 0; i < MODULUS; i++)
	{
		if(map->entries[i] == NULL)
			continue;

		// remove entire chain
		struct entry *current = map->entries[i];
		while(current != NULL)
		{
			free(current->key);
			free(current->value);
			struct entry *previous = current;
			current = current->next;
			free(previous);
		}
	}

	free(map->entries);
	free(map);
	return;
}
