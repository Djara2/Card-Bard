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

	return 0;
}
