#include "card.h"
#include <stdbool.h>

#ifndef CARD_SET_H
#define CARD_SET_H

struct card_set
{
	char *name;
	unsigned char name_length;  // Probably will not need more than 255 chars for a title.
	struct card **cards;
	unsigned short length;      // Probably will not need more than 32,000 cards.
	unsigned short capacity; 
};

// Creation methods
struct card_set* card_set_create(char *name);

// Modification methods
bool card_set_add(struct card_set *set, struct card *c);

bool card_set_remove_by_key(struct card_set *set, char *key);

bool card_set_remove_by_index(struct card_set *set, unsigned short index);

// File IO
bool card_set_write_binary(struct card_set *set);

bool card_set_write_csv(struct card_set *set);

struct card_set* card_set_read_binary(char *file_name);

struct card_set* card_set_read_csv(char *file_name);

#endif
