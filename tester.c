#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashmap.h"

int main(void)
{
	// TEST ONE --- ENTRY CREATION 
	printf("Test 1: Entry creation.\n");
	char **expected_output1 = malloc(sizeof(char*) * 2);
	if(expected_output1 == NULL)
	{
		fprintf(stderr, "Failed to allocate expected output buffer for 1st test.\n");
		return 1;
	}

	expected_output1[0] = malloc(strlen("card") + 1);
	expected_output1[1] = malloc(strlen("bard") + 1);
	strcpy(expected_output1[0], "card");
	strcpy(expected_output1[1], "bard");

	struct entry *new_entry = entry_create(expected_output1[0], expected_output1[1]);
	if(new_entry == NULL)
	{
		fprintf(stderr, "\tTest FAILED. Entry was NULL after call \"struct entry *new_entry = entry_create(expected_output1[0], expected_output1[1]);\", but should not have been.\n");
		return 1;
	}
	printf("\tSUCCESS: Entry was not NULL.\n");

	if(strcmp(new_entry->key, expected_output1[0]) != 0)
	{
		fprintf(stderr, "\tTest FAILED. Entry's key attribute was expected to have value \"%s\", but instead had \"%s\".\n", expected_output1[0], new_entry->key);
		free(new_entry->key);
		free(new_entry->value);
		free(new_entry);
		return 1;
	}
	printf("\tSUCCESS: Entry's key attribute matched correct value.\n");

	if(strcmp(new_entry->value, expected_output1[1]) != 0)
	{
		fprintf(stderr, "\tTest FAILED. Entry's value attribute was expected to have value \"%s\", but instead had \"%s\".\n", expected_output1[1], new_entry->value);
		free(new_entry->key);
		free(new_entry->value);
		free(new_entry);
		return 1;
	}
	printf("\tSUCCESS: Entry's value attribute matched correct value.\n");
	printf("Test PASSED!\n\n");
	free(new_entry->key);
	free(new_entry->value);
	free(new_entry);

	// TEST TWO --- HASHMAP CREATION AND INSERTION
	struct hashmap *map = hashmap_create();	
	printf("Test 2: Entry creation.\n");
	if(map == NULL)
	{
		fprintf(stderr, "\tTest FAILED. Hashmap_create() returned NULL.\n");
		return 1;
	}
	printf("\tSUCCESS: hashmap was not NULL.\n");
	unsigned int hash = hash_string("card");
	bool status;
	status = hashmap_insert(map, "card", "bard");
	if(status == false)
	{
		fprintf(stderr, "Test FAILED. False status returned for \"bool hashmap_insert(struct hashmap *map, char *key, char *value);\". The expected value was true.\n");
		free(map);
		return 1;
	}
	printf("\tSUCCESS: True return status for \"void entry_destroy(struct entry *e);\".\n");

	if(strcmp(map->entries[hash]->key, "card") != 0)
	{
		fprintf(stderr, "Test FAILED. Inserted KEY does not match expected value.\n");
		entry_destroy(map->entries[hash]); // okay to use entry_destroy() here, because there is no chaining after inserting just one entry.
		free(map->entries);
		free(map);
		return 1;
	}
	printf("\tSUCCESS: Correct value \"%s\" was inserted to hashmap at index %u.\n", "card", hash);

	if(strcmp(map->entries[hash]->value, "bard") != 0)
	{
		fprintf(stderr, "Test FAILED. Inserted VALUE does not match expected value for key \"%s\"\n", "card");
		entry_destroy(map->entries[hash]); // okay to use entry_destroy() here, because there is no chaining after inserting just one entry.
		free(map->entries);
		free(map);
		return 1;
	}
	printf("\tSUCCESS: Correct value \"%s\" was inserted to hashmap at index %u.\n", "card", hash);

	printf("Test PASSED!.\n");
	free(map);

	return 0;
}
