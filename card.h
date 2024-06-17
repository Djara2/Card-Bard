#ifndef CARD_H
#define CARD_H
struct card
{
	char *key;
	char **values;                   // (1) Allow multiple valid responses
	unsigned char values_length;     // (2) There should not be more than 256	                                
					 //     valid answers for one single card.
};

struct card* card_create(char *key, char *value);

struct card* card_create_many_answers(char *key, char **values, unsigned char length);

bool card_add_values(struct card *card, char *value);

void card_destroy(struct card *e);

#endif
