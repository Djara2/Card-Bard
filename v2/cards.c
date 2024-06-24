#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cards.h"
#include <stdarg.h>
#define byte unsigned char

void card_destroy(struct card *c)
{
	if(c == NULL)
	{
		fprintf(stderr, "The provided card was NULL.\n");
		return;
	}

	if(c->front != NULL)
		free(c->front);

	if(c->back != NULL)
		free(c->back);

	if(c->alternate_answers != NULL && c->alternate_answers_length > 0)
	{
		for(byte i = 0; i < c->alternate_answers_length; i++)
			free(c->alternate_answers[i]);

		free(c->alternate_answers);
	}

	free(c);
	return;
}

// Rest of the parameters are for the alternate answers buffer.
struct card* card_create(char *front, char *back, byte va_list_length, ...)
{
	va_list args;
	va_start(args, va_list_length);

	struct card *new_card = malloc(sizeof(struct card));
	if(new_card == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for a card struct. Function call was card_create(\"%s\", \"%s\", ...). Note that the variable arguments (va_list members) were not displayed here.\n", front, back);
		va_end(args);
		return NULL;
	}

	// Copy over the front member
	new_card->front = malloc(sizeof(char) * (strlen(front) + 1));
	if(new_card->front == NULL)
	{ 
		fprintf(stderr, "Failed to allocate memory for the front member of the card struct. Function call was Function call was card_create(\"%s\", \"%s\", ...). Note that the variable arguments (va_list members) were not displayed here.\n", front, back);
		va_end(args);
		free(new_card); 
		return NULL;
	}
	strcpy(new_card->front, front);

	// Copy over the back member
	new_card->back = malloc(sizeof(char) * (strlen(back) + 1));
	if(new_card->back == NULL)
	{ 
		fprintf(stderr, "Failed to allocate memory for the back member of the card struct. Function call was Function call was card_create(\"%s\", \"%s\", ...). Note that the variable arguments (va_list members) were not displayed here.\n", front, back);
		free(new_card->front);
		free(new_card);
		va_end(args);
		return NULL;
	}
	strcpy(new_card->back, back);

	// Copy over each alternate answer
	new_card->alternate_answers_length = va_list_length;
	if(va_list_length > 0)
	{
		new_card->alternate_answers = malloc(sizeof(char*) * va_list_length);
		for(int i = 0; i < va_list_length; i++)
		{
			unsigned short alternate_answer_length;
			char *alternate_answer;
			alternate_answer = va_arg(args, char*);
			alternate_answer_length = strlen(alternate_answer);
			new_card->alternate_answers[i] = malloc(sizeof(char) * (alternate_answer_length + 1));
			if(new_card->alternate_answers[i] == NULL)
			{
				card_destroy(new_card);
				return NULL;
			}
			strcpy(new_card->alternate_answers[i], alternate_answer);
		}
	}
	else new_card->alternate_answers = NULL;

	return new_card;
}

void card_print(struct card *c)
{
	if(c == NULL)
	{
		fprintf(stderr, "Cannot print a NULL card.\n");
		return;
	}

	if(c->front == NULL)
		printf("<no front data>.\n");
	else
		printf("\"%s\".\n", c->front);

	if(c->back == NULL)
		printf("\t-> <no back data>.\n");
	else
		printf("\t-> \"%s\"\n", c->back);

	if(c->alternate_answers_length > 0)
	{
		for(byte i = 0; i < c->alternate_answers_length; i++)
			printf("\t-> \"%s\"\n", c->alternate_answers[i]);
	}

	return;
}

void card_list_print(struct card **cards, unsigned short length)
{
	if(cards == NULL)
	{
		fprintf(stderr, "Cannot print an array of empty cards.\n");
		return;
	}
	if(length <= 0)
	{
		fprintf(stderr, "Cannot print an array of cards with non-positive length.\n");
		return;
	}

	for(unsigned short i = 0; i < length; i++)
		card_print(cards[i]);

	return;
}

void card_set_destroy(struct card_set *cs)
{
	if(cs == NULL)
	{
		fprintf(stderr, "Cannot destroy a card set that points to NULL.\n");
		return;
	}

	if(cs->name != NULL)			// free name buffer
		free(cs->name);

	if(cs->description != NULL)		// free description buffer
		free(cs->description);

	if(cs->cards != NULL)			// free cards
	{
		for(unsigned short i = 0; i < cs->length; i++)
			card_destroy(cs->cards[i]);

		free(cs->cards);
	}

	free(cs);
	return;
}

// Name is required, but description can be NULL
struct card_set* card_set_create(char *name, char *description)
{
	// Check required parameters
	if(name == NULL)
	{
		fprintf(stderr, "Cannot create a card_set struct without a name.\n");
		return NULL;
	}

	// Allocate memory for card_set struct
	struct card_set *new_card_set = malloc(sizeof(struct card_set));
	if(new_card_set == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for a new card_set struct. Failed on call \"card_set_create(\"%s\"", name);
		if(description == NULL)
			fprintf(stderr, ", NULL);\n");
		else
			fprintf(stderr, ", \"%s\");\n", description);

		return NULL;
	}

	// Set name member
	new_card_set->name = malloc(strlen(name) + 1);
	if(new_card_set->name == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for the name member of the card_set struct. Failed on call \"card_set_create(\"%s\"", name);
		if(description == NULL)
			fprintf(stderr, ", NULL);\n");
		else
			fprintf(stderr, ", \"%s\");\n", description);

		free(new_card_set);
		return NULL;
	}
	strcpy(new_card_set->name, name);

	// Set description member
	if(description != NULL)
	{
		new_card_set->description = malloc(strlen(description) + 1);
		if(new_card_set->description == NULL)
		{
			fprintf(stderr, "Failed to allocate memory for the name member of the card_set struct. Failed on call \"card_set_create(\"%s\", \"%s\");", name, description);
			free(new_card_set->name);
			free(new_card_set);
			return NULL;
		}
		strcpy(new_card_set->description, description);
	}
	else new_card_set->description = NULL;

	// Allocate base number of 10 cards for cards buffer
	new_card_set->cards = malloc(sizeof(struct card*) * 10);
	if(new_card_set->cards == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for the cards buffer of the card_set struct. Failed on call \"card_set_create(\"%s\", ", name);
		if(description != NULL)
			fprintf(stderr, "\"%s\");\n", description);
		else
			fprintf(stderr, "NULL);\n");

		card_set_destroy(new_card_set);
		return NULL;
	}

	// Set cards buffer information members 
	new_card_set->length = 0;
	new_card_set->capacity = 10;

	return new_card_set;
}

// Same thing as card_create method, but adding to the cards buffer is automatically handled
bool card_set_add(struct card_set *cs, char *front, char *back, byte va_list_length, ...)
{
	if(cs == NULL)
	{
		fprintf(stderr, "Cannot add new cards to a card set struct that points to NULL.\n");
		return false;
	}

	// An unsigned short is used for the length and capacity of the cards buffer, so if the maximum
	// size that can be represented by an unsigned short (32,767) has been reached, nothing can be done.
	if(cs->length >= 32767)
	{
		fprintf(stderr, "The card set is too big. It has reached a size of 32,767.\n");
		return false;
	}

	va_list args;
	va_start(args, va_list_length);

	// Ensure there is enough space to add more cards
	if(cs->length >= cs->capacity)
	{
		cs->capacity += 50;
		if(cs->capacity > 32767)
			cs->capacity = 32767;
		cs->cards = realloc(cs->cards, sizeof(struct card*) * cs->capacity);
		if(cs->cards == NULL)
		{
			fprintf(stderr, "Failed to reallocate more memory for the cards buffer of the card set. This failure occurred with length %hu and capacity %hu.\n", cs->length, cs->capacity);
			fprintf(stderr, "Because the cards buffer is now NULL, the card set will be destroyed.\n");
			card_set_destroy(cs);
			va_end(args);
			return false;
		}
	}
	
	struct card **cards = cs->cards;

	// Allocate the new card struct
	cards[cs->length] = malloc(sizeof(struct card));
	if(cards[cs->length] == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for a card struct at index %hu of the card set's card buffer. This failure happened with length %hu and capacity %hu.\n", cs->length, cs->capacity);
		va_end(args);
		return false;
	}

	// Copy over the front member
	cards[cs->length]->front = malloc(sizeof(char) * (strlen(front) + 1));
	if(cards[cs->length]->front == NULL)
	{ 
		fprintf(stderr, "[card_set_add] Failed to allocate memory for the front member of the card struct.\n");
		va_end(args);
		card_destroy(cards[cs->length]);
		return false;
	}
	strcpy(cards[cs->length]->front, front);

	// Copy over the back member
	cards[cs->length]->back = malloc(sizeof(char) * (strlen(back) + 1));
	if(cards[cs->length]->back == NULL)
	{ 
		fprintf(stderr, "[card_set_add] Failed to allocate memory for the back member of the card struct.");
		card_destroy(cards[cs->length]);
		va_end(args);
		return false;
	}
	strcpy(cards[cs->length]->back, back);

	// Copy over each alternate answer
	cards[cs->length]->alternate_answers_length = va_list_length;
	if(va_list_length > 0)
	{
		cards[cs->length]->alternate_answers = malloc(sizeof(char*) * va_list_length);
		for(int i = 0; i < va_list_length; i++)
		{
			unsigned short alternate_answer_length;
			char *alternate_answer;
			alternate_answer = va_arg(args, char*);
			alternate_answer_length = strlen(alternate_answer);
			cards[cs->length]->alternate_answers[i] = malloc(sizeof(char) * (alternate_answer_length + 1));
			if(cards[cs->length]->alternate_answers[i] == NULL)
			{
				card_destroy(cards[cs->length]);
				return false;
			}
			strcpy(cards[cs->length]->alternate_answers[i], alternate_answer);
		}
	}
	else cards[cs->length]->alternate_answers = NULL;

	cs->length++;
	return true;
}

bool card_set_add_card(struct card_set *cs, struct card *c)
{
	if(cs == NULL)
	{
		fprintf(stderr, "Cannot add new cards to a card set struct that points to NULL.\n");
		return false;
	}

	if(c == NULL)
	{
		fprintf(stderr, "Cannot add a card that points to NULL.\n");
		return false;
	}

	// An unsigned short is used for the length and capacity of the cards buffer, so if the maximum
	// size that can be represented by an unsigned short (32,767) has been reached, nothing can be done.
	if(cs->length >= 32767)
	{
		fprintf(stderr, "The card set is too big. It has reached a size of 32,767.\n");
		return false;
	}

	// Ensure there is enough space to add more cards
	if(cs->length >= cs->capacity)
	{
		cs->capacity += 50;
		if(cs->capacity > 32767)
			cs->capacity = 32767;
		cs->cards = realloc(cs->cards, sizeof(struct card*) * cs->capacity);
		if(cs->cards == NULL)
		{
			fprintf(stderr, "Failed to reallocate more memory for the cards buffer of the card set. This failure occurred with length %hu and capacity %hu.\n", cs->length, cs->capacity);
			fprintf(stderr, "Because the cards buffer is now NULL, the card set will be destroyed.\n");
			card_set_destroy(cs);
			return false;
		}
	}

	// Add card
	cs->cards[cs->length] = c;
	cs->length++;
	return true;
}

void card_set_print(struct card_set *cs)
{
	if(cs == NULL)
	{
		fprintf(stderr, "Cannot print the contents of a card set struct that points to NULL.\n"); 
		return;
	}

	printf("Name: ");
	if(cs->name == NULL)
	{
		fprintf(stderr, "Card set does not have a name (points to NULL). Replacing name with \"???\".\n");
		printf("???\n");
	}
	else printf("%s\n", cs->name);

	printf("Description: ");
	if(cs->description != NULL)
		printf("%s\n", cs->description);

	printf("Cards: %hu\n", cs->length);
	if(cs->cards == NULL)
		return;

	for(unsigned short i = 0; i < cs->length; i++)
	{
		printf("Card %hu: ", i);
		card_print(cs->cards[i]);
		printf("\n");
	}
	return;
}
