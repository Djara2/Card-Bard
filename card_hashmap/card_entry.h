#ifndef CARD_ENTRY_H
#define CARD_ENTRY_H
struct card_entry 
{
	char *key;
	char **values;                   // (1) Allow multiple valid responses
	unsigned char values_length;     // (2) There should not be more than 256
	                                 //     valid answers for one single card.
	struct card_entry *next;         // (3) Linear probing
};

struct card_entry* card_entry_create(char *key, char *value);

struct card_entry* card_entry_create_many_answers(char *key, char **values, unsigned char length);

bool card_entry_add_values(struct card_entry *card, char *value);

void card_entry_destroy(struct card_entry *e);

#endif
