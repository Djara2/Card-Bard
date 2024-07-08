#include <stdbool.h>
#define byte unsigned char
#ifndef CARDS_H
#define CARDS_H

struct card
{
	char *front;			// Card prompt
	char *back;			// Answer
	char **alternate_answers;	// Other acceptable answers
	byte alternate_answers_length;	
};

struct card_set
{
	char *name;
	char *description;
	struct card **cards;
	unsigned short length;
	unsigned short capacity;
};

void card_destroy(struct card *c);

struct card* card_create(char *front, char *back, byte va_list_length, ...);

struct card* card_create_from_array(char **data, byte data_length);

bool card_validate_answer(struct card *c, char *answer);

void card_print(struct card *c);

void card_list_print(struct card **cards, unsigned short length);

void card_set_destroy(struct card_set *cs);

struct card_set* card_set_create(char *name, char *description);

bool card_set_add(struct card_set *cs, char *front, char *back, byte va_list_length, ...);

bool card_set_add_card(struct card_set *cs, struct card *c);

void slice_string(char *str, unsigned short left_offset, unsigned short right_offset);

void get_tokens(char *str, char *delimiter, char ***tokens, byte *tokens_length, byte *tokens_capacity);

struct card_set* card_set_create_from_csv(char *file_name);

void card_set_print(struct card_set *cs);

unsigned short card_set_play(struct card_set *cs);

unsigned short card_set_play_random(struct card_set *cs);

#endif
