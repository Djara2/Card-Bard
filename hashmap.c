#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

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
