#ifndef CARD_HASHMAP_H
#define CARD_HASHMAP_H
#define SCALAR 31
#define MODULUS 101
#include <stdbool.h>
#include "card_entry.h"

unsigned int hash_string(char *input);

struct hashmap
{
	struct card_entry **entries;
	unsigned short length; // max size attribute is not required because of the MODULUS macro.
};

struct hashmap* hashmap_create();

// Card with only one valid answer
bool hashmap_insert(struct hashmap *map, char *key, char *value);

// Card with several valid answers
bool hashmap_insert_key_and_array(struct hashmap *map, char *key, char **values, unsigned char length);

bool hashmap_contains(struct hashmap *map, char *key);

bool hashmap_remove(struct hashmap *map, char *key);

void hashmap_destroy(struct hashmap *map);

#endif

