#ifndef HASHMAP_H
#define HASHMAP_H
#define SCALAR 31
#define MODULUS 101
#include <stdbool.h>

unsigned int hash_string(char *input);

struct entry 
{
	char *key;
	char *value;
	struct entry *next; // linear probing
};

struct entry* entry_create(char *key, char *value);

void entry_destroy(struct entry *e);

struct hashmap
{
	struct entry **entries;
	unsigned short length; // max size attribute is not required because of the MODULUS macro.
};

struct hashmap* hashmap_create();

bool hashmap_insert(struct hashmap *map, char *key, char *value);

#endif
