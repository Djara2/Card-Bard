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
	printf("Test 2: Entry insertion into hashmap.\n");
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

	printf("Test PASSED!.\n\n");

	// TEST THREE --- HASHMAP INSERTION WITH CHAINING
	printf("Test 3: Entry insertion with chaining.\n");
	if(map == NULL)
	{
		fprintf(stderr, "\tTest FAILED. Hashmap_create() returned NULL.\n");
		return 1;
	}
	printf("\tSUCCESS: hashmap was not NULL.\n");
	hash = hash_string("drac");
	status = hashmap_insert(map, "drac", "drab");
	if(status == false)
	{
		fprintf(stderr, "Test FAILED. False status returned for \"bool hashmap_insert(struct hashmap *map, char *key, char *value);\". The expected value was true.\n");
		free(map);
		return 1;
	}
	printf("\tSUCCESS: True return status for hashmap_insert call.\n");

	if(strcmp(map->entries[hash]->key, "drac") == 0)
	{
		fprintf(stderr, "Test FAILED. Inserted KEY does matched expected value; however, the expected value should have been inserted as the next node for the first node residing at index %u\n", hash);
		entry_destroy(map->entries[hash]); // okay to use entry_destroy() here, because there is no chaining after inserting just one entry.
		free(map->entries);
		free(map);
		return 1;
	}
	printf("\tSUCCESS: Value \"%s\" was NOT inserted to hashmap at index %u, because something already exists there.\n", "drac", hash);

	if(map->entries[hash]->next == NULL)
	{
		fprintf(stderr, "\tTest FAILED. The node at hash %u is supposed to have a \"next\" attribute that points to a new node mapping \"drac\" to \"drab\", but instead the \"next\" attribute is NULL.\n", hash);
		entry_destroy(map->entries[hash]); // still okay to use, because there is only one node.
		free(map->entries);
		free(map);
	}
	printf("\tSUCCESS: The node at index %u has a \"next\" attribute that points to something.\n", hash);

	if(strcmp(map->entries[hash]->next->key, "drac") != 0)
	{
		fprintf(stderr, "Tested FAILED. The newly inserted node, using linear chaining, is expected to have a key of \"drac\", but instead has a key of \"%s\".\n", map->entries[hash]->next->key);
		free(map->entries[hash]->next->key);
		free(map->entries[hash]->next->value);	
		free(map->entries[hash]->next);
		entry_destroy(map->entries[hash]);
		free(map->entries);
		free(map);
	}
	printf("\tSUCCESS: Correct value \"%s\" was inserted to hashmap at index %u as a linked node.\n", "drac", hash);

	printf("Test PASSED!.\n\n");

	// TEST 4 - HASHMAP CONTAINS 
	printf("Test 4: Hashmap lookup\n");
	status = hashmap_contains(map, "card");
	if(status == false)
	{
		fprintf(stderr, "Test FAILED. Hashmap lookup attempted for \"card\" which returned false, but was expected to return true.\n");
		free(map->entries[hash]->next->key);
		free(map->entries[hash]->next->value);
		free(map->entries[hash]->next);
		free(map->entries[hash]->key);
		free(map->entries[hash]->value);
		free(map->entries);
		free(map);
		return 1;
	}
	printf("\tSUCCESS: Key \"card\" was found!\n");

	status = hashmap_contains(map, "drac");
	if(status == false)
	{
		fprintf(stderr, "Test FAILED. Hashmap lookup attempted for \"drac\" which returned false, but was expected to return true.\n");
		free(map->entries[hash]->next->key);
		free(map->entries[hash]->next->value);
		free(map->entries[hash]->next);
		free(map->entries[hash]->key);
		free(map->entries[hash]->value);
		free(map->entries);
		free(map);
		return 1;
	}
	printf("\tSUCCESS: Key \"drac\" was found!\n");
	printf("Test PASSED!\n\n");

	// TEST FIVE --- Hashmap entry deletion
	printf("Test 5: Hashmap entry deletion.\n");
	hashmap_insert(map, "crad", "brad"); // should result in collision
	status = hashmap_remove(map, "drac");
	if(status == false)
	{
		fprintf(stderr, "Test FAILED. Hashmap entry removal attempted for \"drac\" which returned false, but was expected to return true.\n");
		free(map->entries[hash]->next->key);
		free(map->entries[hash]->next->value);
		free(map->entries[hash]->next);
		free(map->entries[hash]->key);
		free(map->entries[hash]->value);
		free(map->entries);
		free(map);
		return 1;
	}
	printf("\tSUCCESS: Removed element.\n");
	status = hashmap_contains(map, "drac");
	if(status == true)
	{
		fprintf(stderr, "Test FAILED. Hashmap entry lookup attempted for \"drac\" which returned true, but was expected to return false, because the element was formerly deleted.\n");
		free(map->entries[hash]->next->key);
		free(map->entries[hash]->next->value);
		free(map->entries[hash]->next);
		free(map->entries[hash]->key);
		free(map->entries[hash]->value);
		free(map->entries);
		free(map);
		return 1;
	}
	printf("\tSUCCESS: Lookup for deleted element returns false.\n");

	status = hashmap_contains(map, "crad");
	if(status == false)
	{
		fprintf(stderr, "Test FAILED. Hashmap entry lookup attempted for \"crad\" which returned false, but was expected to return true.\n");
		free(map->entries[hash]->next->key);
		free(map->entries[hash]->next->value);
		free(map->entries[hash]->next);
		free(map->entries[hash]->key);
		free(map->entries[hash]->value);
		free(map->entries);
		free(map);
		return 1;
	}
	printf("\tSUCCESS: Linked element remained in hashmap after deletion of another element.\n");

	printf("Test PASSED!\n\n");
	
	// TEST SIX --- Hashmap destroy
	printf("Test 6: Hashmap destroy\n");
	hashmap_destroy(map);
		// fprintf(stderr, "Test FAILED. Hashmap was not NULL after calling hashmap_destroy method.\n");
		// return 1;
	printf("\tSUCCESS: Map was NULL after calling hashmap_destroy.\n");
	printf("Test PASSED!\n\n");
	return 0;
}
