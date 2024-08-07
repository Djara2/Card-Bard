#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cards.h"
#include <stdarg.h>
#include <time.h>

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

// No va_list. Extra answers are passed as an array. Used in card_set_create_from_csv method.
struct card* card_create_with_array(char **data, byte data_length)
{
	if(data_length < 2)
	{
		fprintf(stderr, "Cannot create a card from a data buffer with less than 2 members. A front and back side are the minimum requirements for a card.\n");
		fprintf(stderr, "Data buffer contents were:");
		for(int i = 0; i < data_length; i++)
			printf("%d. %s\n", i, data[i]);

		return NULL;
	}

	struct card *new_card = malloc(sizeof(struct card));
	if(new_card == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for a card struct. Function call was card_create(\"%s\", \"%s\", ...). Note that the variable arguments (va_list members) were not displayed here.\n", data[0], data[1]);
		return NULL;
	}

	// Set up base members	
	char *front = data[0];
	char *back = data[1];

	// Copy over the front member
	new_card->front = malloc(sizeof(char) * (strlen(front) + 1));
	if(new_card->front == NULL)
	{ 
		fprintf(stderr, "Failed to allocate memory for the front member of the card struct. Function call was Function call was card_create(\"%s\", \"%s\", ...). Note that the variable arguments (va_list members) were not displayed here.\n", front, back);
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
		return NULL;
	}
	strcpy(new_card->back, back);

	// Copy over each alternate answer
	byte alternate_answers_length = data_length - 2;	// exclude front and back
	new_card->alternate_answers_length = alternate_answers_length;
	if(alternate_answers_length > 0)
	{
		new_card->alternate_answers = malloc(sizeof(char*) * alternate_answers_length);
		for(int i = 2; i < data_length; i++)
		{
			unsigned short alternate_answer_length = strlen(data[i]);

			// Allocate alternate answer
			new_card->alternate_answers[i-2] = malloc(sizeof(char) * (alternate_answer_length + 1));
			if(new_card->alternate_answers[i-2] == NULL)
			{
				fprintf(stderr, "Failed to allocate memory for alternate answer to prompt \"%s\".\n", front);
				card_destroy(new_card);
				return NULL;
			}
			strcpy(new_card->alternate_answers[i-2], data[i]);
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

bool card_validate_answer(struct card *c, char *answer)
{
	if(c == NULL)
	{
		fprintf(stderr, "Cannot validate an answer for a card that points to NULL.\n");
		return false;
	}

	if(c->back == NULL)
	{
		fprintf(stderr, "Cannot validate an answer for a card that points to NULL.\n");
		return false;
	}

	// Check back with answer
	if(strcmp(c->back, answer) == 0)
		return true;

	// Are there alternate answers if the primary answer didn't match?
	if(c->alternate_answers == NULL || c->alternate_answers_length <= 0)
		return false;

	// Check alternate answers
	for(byte i = 0; i < c->alternate_answers_length; i++)
	{
		if(strcmp(c->alternate_answers[i], answer) == 0)
			return true;
	}

	return false;
}

bool card_validate_answer_back(struct card *c, char *answer)
{
	if(c == NULL)
	{
		fprintf(stderr, "Cannot validate an answer for a card that points to NULL.\n");
		return false;
	}

	if(c->front == NULL)
	{
		fprintf(stderr, "Cannot validate an answer for a card that points to NULL.\n");
		return false;
	}

	// Check FRONT with answer
	if(strcmp(c->front, answer) == 0)
		return true;

	return false;
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

// Slices string in place ! Will change value of string ! Use a duplicate of the string to avoid alteration.
void slice_string(char *str, unsigned short left_offset, unsigned short right_offset)
{
	// String cannot be NULL.
	if(str == NULL)
		return;

	// Slice offsets cannot be negative numbers 
	if(left_offset < 0 || right_offset < 0)
	{
		fprintf(stderr, "slice_string: Cannot have left or right offset be a negative number (args: str: \"%s\", left: %hu, right: %hu)\n", str, left_offset, right_offset);
		return;
	}

	// Length of string cannot be 0 or less
	unsigned short source_length = strlen(str);
	if(source_length <= 0)
	{
		fprintf(stderr, "slice_string: Cannot slice a string with length 0.\n");
	}

	// Left offset cannot be greater than length of string
	if(left_offset >= source_length)
	{
		fprintf(stderr, "slice_string: Left offset cannot be larger than the length of the string (args: str: \"%s\", left: %hu, right: %hu)\n", str, left_offset, right_offset);
		return;
	}

	// Right offset cannot be greater than length of string
	if(right_offset >= source_length)
	{
		fprintf(stderr, "slice_string: Right offset cannot be larger than the length of the string (args: str: \"%s\", left: %hu, right: %hu)\n", str, left_offset, right_offset);
		return;
	}

	// Offsets cannot remove too many characters
	unsigned short new_length = source_length - (left_offset + right_offset);
	if(new_length <= 0)
	{
		fprintf(stderr, "slice_string: Left and right offsets cannot remove more characters than exist within the source string. (args: str; \"%s\", left: %hu, right: %hu)\n", str, left_offset, right_offset);
	}

	// Modify string in place
	for(unsigned short i = 0; i < new_length; i++)
		str[i] = str[left_offset + i];

	str[new_length] = '\0';		// null terminate string
	return;
}

/* This will REMOVE leading whitespace so that you can write CSVs like 
 * 1 term, answer, answer 2, answer 3, ...
*/
void get_tokens(char *str, char *delimiter, char ***tokens, byte *tokens_length, byte *tokens_capacity)
{
	char *token;
	token = strtok(str, delimiter);
	while(token != NULL)
	{
		// Ensure there is sufficient space
		if( (*tokens_length) >= (*tokens_capacity) )
		{
			(*tokens_capacity) *= 2;
			(*tokens) = realloc((*tokens), sizeof(char*) * (*tokens_capacity));
			if( (*tokens) == NULL )
			{
				fprintf(stderr, "Failed to reallocate the tokens buffer to accomodate more tokens.\n");
				return;
			}
		}

		// Remove leading whitespace from the token
		unsigned short offset = 0;
		while(token[offset] == ' ')
			offset++;

		if(offset > 0)
		{
			printf("Turned \"%s\" into ", token);
			slice_string(token, offset, 0);
			printf("\"%s\"\n", token);	
		}

		// Copy the token into the tokens buffer
		(*tokens)[(*tokens_length)] = malloc(strlen(token) + 1);
		strcpy( (*tokens)[(*tokens_length)], token );
		(*tokens_length)++;

		// Get next token
		token = strtok(NULL, delimiter); 
	}
	return;
}

// Expects (1) the first line to be the name of the card set
//     and (2) the second line to be the description.
//     The rest of the file is a CSV of form front,back,alt ans 1,alt ans 2,...,alt ans n
struct card_set* card_set_create_from_csv(char *file_name)
{
	// Open file
	FILE *fh = fopen(file_name, "r");
	if(fh == NULL)
	{
		fprintf(stderr, "Failed to open file \"%s\".\n", file_name);
		return NULL;
	}

	// Create line buffer
	char *line = malloc(256);
	if(line == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for the line buffer while reading contents of \"%s\".\n", file_name);
		fclose(fh);
		return NULL;
	}
	unsigned short line_length = 0;
	unsigned short line_capacity = 256;

	// Create tokens buffer for analyzing the line
	char **tokens = malloc(sizeof(char*) * 5);		// There cannot be more than
	byte tokens_length = 0;					// 256 tokens in a line
	byte tokens_capacity = 5;
	if(tokens == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for the tokens buffer while reading the contents of \"%s\".\n", file_name);
		free(line);
		fclose(fh);
		return NULL;
	}

	// Create card set struct
	struct card_set *cs = malloc(sizeof(struct card_set));
	if(cs == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for the card_set struct while reading contents of \"%s\".\n", file_name); 
		free(line); 
		free(tokens);
		fclose(fh);
		return NULL;
	}

	// Begin reading first 2 lines for name of set and des
	char c = 1;
	unsigned short line_number = 0; 
	while(c != EOF)
	{
		c = fgetc(fh);

		// Stop reading the line and update name or description member
		if(c == '\n' || c == EOF)
		{
			// Cut off the line
			line[line_length] = '\0';
			line_length++;

			// Set name or description member
			if(line_number == 0)
			{
				// Allocate the name buffer
				cs->name = malloc(strlen(line) + 1);
				if(cs->name == NULL)
				{
					fprintf(stderr, "Failed to allocate memory for the name buffer of the card set while reading the contents of \"%s\".\n", file_name);
					free(cs);
					free(line);
					free(tokens);
					fclose(fh);
					return NULL;
				}

				// Copy the name buffer data
				strcpy(cs->name, line);

				// Reset line data for the next line
				line_length = 0;
				line_number++;
			}
			else
			{
				// Allocate the description buffer
				cs->description = malloc(strlen(line) + 1);
				if(cs->description == NULL)
				{
					fprintf(stderr, "Failed to allocate memory for the description buffer of the card set while reading the contents of \"%s\".\n", file_name);
					free(cs->name);
					free(cs);
					free(line);
					free(tokens);
					fclose(fh);
					return NULL;
				}

				// Copy the description buffer data
				strcpy(cs->description, line);

				// Reset line data for the next line
				line_length = 0;
				line_number++;

				// break out of this loop to start reading cards
				break;
			}
		}
		// Continue building the line
		else
		{
			// Ensure there is enough space
			if(line_length >= line_capacity)
			{
				line_capacity *= 2;
				line = realloc(line, line_capacity);
				if(line == NULL)
				{
					fprintf(stderr, "Failed to reallocate more memory for the line buffer while reading the contents of \"%s\". Failure on line number %hu.\n", file_name, line_number); 
					if(cs->name != NULL)
						free(cs->name);
					free(line);
					free(tokens);
					free(cs);
					fclose(fh);
					return NULL;
				}
			}

			// Add characters to the line
			line[line_length] = c;
			line_length++;
		}
	}

	// Begin reading contents of CSV to store into card set
	while(c != EOF)
	{
		c = fgetc(fh);

		// Stop reading line and convert to a card when the end of the line is reached.
		if(c == '\n' || c == EOF)
		{
			// Cut off the line
			line[line_length] = '\0';
			line_length++;

			// Tokenize the line
			get_tokens(line, ",", &tokens, &tokens_length, &tokens_capacity);

			// DEBUGGING
			printf("for line %hu:\n", line_number);
			for(byte i = 0; i < tokens_length; i++)
				printf("%s ", tokens[i]);
			printf("\n");

			// Analyze the tokens --- Create a card from the tokens
			if(tokens_length <= 0)
				break;

			struct card *new_card = card_create_with_array(tokens, tokens_length);
			if(new_card == NULL)
			{
				fprintf(stderr, "Failed to create a new card using the tokens found while reading file \"%s\".\n", file_name);
				card_set_destroy(cs);
				free(line);
				for(byte i = 0; i < tokens_length; i++)
					free(tokens[i]); 
				free(tokens);
				fclose(fh);
				return NULL;
			}

			// Add card to the card set
			bool add_status = false;
			if(tokens_length > 0)
				add_status = card_set_add_card(cs, new_card);

			if(add_status != true)
				fprintf(stderr, "Failed to add card #%hu while reading file \"%s\".\n", line_number - 2, file_name);

			// Increase line number and reset the line for the next reading
			line_length = 0;
			line_number++;

			// Reset the tokens buffer
			tokens_length = 0;
		}
		else
		{
			// Ensure there is enough space
			if(line_length >= line_capacity)
			{
				line_capacity *= 2;
				line = realloc(line, line_capacity);
				if(line == NULL)
				{
					fprintf(stderr, "Failed to reallocate more memory for the line buffer while reading the contents of \"%s\". Failure on line number %hu.\n", file_name, line_number); 
					card_set_destroy(cs);
					free(line);
					for(byte i = 0; i < tokens_length; i++)
						free(tokens[i]);
					free(tokens);
					fclose(fh);
					return NULL;
				}
			}

			// Add characters to the line
			line[line_length] = c;
			line_length++;
		}
	}

	// Return the card set that was created
	return cs;
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

// Returns the amount of prompts answered correctly (incorrect amount can then be deduced)
unsigned short card_set_play(struct card_set *cs, unsigned short *incorrect_prompts)
{
	// Setup for user input
	char *input_buffer = malloc(25);
	char input_c;
	size_t input_buffer_length = 0;
	size_t input_buffer_capacity = 25;

	// Statistics
	unsigned short incorrect_counter = 0;

	for(unsigned short i = 0; i < cs->length; i++)
	{
		// Reset input buffer length
		input_buffer_length = 0;
		input_c = 1;

		// Prompt the user with the question
		printf("(%hu/%hu) %s\n> ", i + 1, cs->length, cs->cards[i]->front);

		// get user input
		while(input_c != '\n')
		{
			// Get a character from stdin
			input_c = getc(stdin);

			// Make sure buffer is big enough to append it
			if(input_buffer_length >= input_buffer_capacity)
			{
				input_buffer_capacity *= 2;
				input_buffer = realloc(input_buffer, input_buffer_capacity);
				if(input_buffer == NULL)
				{
					fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
					return -1;
				}
			}
			if(input_c == '\n')
				continue;
			
			input_buffer[input_buffer_length] = input_c;
			input_buffer_length++;
		}
		input_buffer[input_buffer_length] = '\0';
		input_buffer_length++;

		// Validate answer
		if(card_validate_answer(cs->cards[i], input_buffer))
			printf("[CORRECT!]\n\n");
		else
		{
			printf("[INCORRECT!]\n\n");

			// Show the user what the correct input was.
			// (1) Only one correct answer
			if(cs->cards[i]->alternate_answers_length == 0)
				printf("The correct answer was \"%s\"", cs->cards[i]->back);

			// (2) Several correct answers
			else
			{
				printf("The acceptable answers are:\n");
				printf("\t1. %s\n", cs->cards[i]->back);
				for(byte j = 0; j < cs->cards[i]->alternate_answers_length; j++)
					printf("\t%d. %s\n", j+2, cs->cards[i]->alternate_answers[j]);
			}

			// Ask the user if they want to override the incorrect labeling.
			printf("\nOverride incorrect marker? [y/n]\n> ");

			// Get user input
			input_c = 1;
			input_buffer_length = 0;
			while(input_c != '\n')
			{
				// Get a character from stdin
				input_c = getc(stdin);

				// Make sure buffer is big enough to append it
				if(input_buffer_length >= input_buffer_capacity)
				{
					input_buffer_capacity *= 2;
					input_buffer = realloc(input_buffer, input_buffer_capacity);
					if(input_buffer == NULL)
					{
						fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
						return -1;
					}
				}
				if(input_c == '\n')
					continue;
				
				input_buffer[input_buffer_length] = input_c;
				input_buffer_length++;
			}
			input_buffer[input_buffer_length] = '\0';
			input_buffer_length++;

			// If no override, then mark the prompt as answered incorrectly.
			if(input_buffer[0] == 'n')
			{
				incorrect_prompts[incorrect_counter] = i;
				incorrect_counter++;
			}

			printf("\n\n");
		}
	
	}
	// Review user's performance
	printf("RECAP:\nYou answered %hu prompts correctly!\n", cs->length - incorrect_counter);
	printf("You answered the following prompts INCORRECTLY [%hu total]: %hu", incorrect_counter, incorrect_prompts[0] + 1); // +1 because the system is keeping track of indices; however, this information is presented to a human, who starts counting at 1.
	for(unsigned short i = 1; i < incorrect_counter; i++)
		printf(", %hu", incorrect_prompts[i] + 1);

	printf("\n");

	// Cleanup
	free(input_buffer);

	return incorrect_counter;
}

// RETURNS: Returns the amount of prompts answered correctly (incorrect amount can then be deduced)
// NOTE:    This version only views specific indices in the card set. This is to be used for reviewing 
//          only the prompts that were answered incorrectly after running through a card set with the 
//          card_set_play() function.
unsigned short card_set_play_indices(struct card_set *cs, unsigned short *indices, unsigned short indices_length, unsigned short *incorrect_prompts)
{
	// Setup for user input
	char *input_buffer = malloc(25);
	char input_c;
	size_t input_buffer_length = 0;
	size_t input_buffer_capacity = 25;

	// Statistics
	unsigned short incorrect_counter = 0;

	for(unsigned short j = 0; j < indices_length; j++)
	{
		// Set index (i) in cs->cards to index (j) in indices buffer.
		// This way, we walk through the indices buffer.
		unsigned short i = indices[j];

		// Reset input buffer length
		input_buffer_length = 0;
		input_c = 1;

		// Prompt the user with the question
		printf("#%hu (%hu/%hu) %s\n> ", i, j + 1, indices_length, cs->cards[i]->front);

		// get user input
		while(input_c != '\n')
		{
			// Get a character from stdin
			input_c = getc(stdin);

			// Make sure buffer is big enough to append it
			if(input_buffer_length >= input_buffer_capacity)
			{
				input_buffer_capacity *= 2;
				input_buffer = realloc(input_buffer, input_buffer_capacity);
				if(input_buffer == NULL)
				{
					fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
					return -1;
				}
			}
			if(input_c == '\n')
				continue;
			
			input_buffer[input_buffer_length] = input_c;
			input_buffer_length++;
		}
		input_buffer[input_buffer_length] = '\0';
		input_buffer_length++;

		// Validate answer
		if(card_validate_answer(cs->cards[i], input_buffer))
			printf("[CORRECT!]\n\n");
		else
		{
			printf("[INCORRECT!]\n\n");

			// Show the user what the correct input was. 
			if(cs->cards[i]->alternate_answers_length == 0)
				printf("The correct answer was \"%s\"", cs->cards[i]->back);
			else
			{
				printf("The acceptable answers are:\n");
				printf("\t1. %s\n", cs->cards[i]->back);
				for(byte j = 0; j < cs->cards[i]->alternate_answers_length; j++)
					printf("\t%d. %s\n", j+2, cs->cards[i]->alternate_answers[j]);
			}

			// Ask the user if they want to override the incorrect labeling.
			printf("\nOverride incorrect marker? [y/n]\n> ");

			// Get user input
			input_c = 1;
			input_buffer_length = 0;
			while(input_c != '\n')
			{
				// Get a character from stdin
				input_c = getc(stdin);

				// Make sure buffer is big enough to append it
				if(input_buffer_length >= input_buffer_capacity)
				{
					input_buffer_capacity *= 2;
					input_buffer = realloc(input_buffer, input_buffer_capacity);
					if(input_buffer == NULL)
					{
						fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
						return -1;
					}
				}
				if(input_c == '\n')
					continue;
				
				input_buffer[input_buffer_length] = input_c;
				input_buffer_length++;
			}
			input_buffer[input_buffer_length] = '\0';
			input_buffer_length++;

			if(input_buffer[0] == 'n')
			{
				incorrect_prompts[incorrect_counter] = i;
				incorrect_counter++;
			}

			printf("\n\n");
		}	
	}

	// Review user's performance
	printf("\nRECAP:\nYou answered %hu prompts correctly!\n", cs->length - incorrect_counter);
	if(incorrect_counter != 0)
	{
		printf("You answered the following prompts INCORRECTLY: %hu", incorrect_prompts[0] + 1); // +1 because system uses indices internally; however, this information is presented to a human who starts counting at 1.
		for(unsigned short i = 1; i < incorrect_counter; i++)
			printf(", %hu", i + 1);

		printf("\n");
	}

	// Cleanup
	free(input_buffer);

	return incorrect_counter;
}

// Returns the amount of prompts answered correctly (incorrect amount can then be deduced)
unsigned short card_set_play_random(struct card_set *cs)
{
	// Set up random seed
	srand(time(NULL));

	// Set up buffer marking everything complete or not
	bool *visited_statuses_buffer = malloc(sizeof(bool) * cs->length);
	if(visited_statuses_buffer == NULL)
	{
		fprintf(stderr, "card_set_play_random: Failed to allocate buffer for marking card visited or not with card set \"%s,\" which has length %hu.\n", cs->name, cs->length);
		return 0;
	}
	for(unsigned short i = 0; i < cs->length; i++)
		visited_statuses_buffer[i] = false;
	
	// Setup for user input
	char *input_buffer = malloc(25);
	char input_c;
	size_t input_buffer_length = 0;
	size_t input_buffer_capacity = 25;

	// Statistics
	unsigned short correct_counter = 0;		// number of correctly answered
	unsigned short *correct_prompts = malloc(sizeof(unsigned short) * (cs->length)); // particular prompts answered correctly. Just assume the user gets them all right, then no extra allocation has to be done.
	if(correct_prompts == NULL)
	{
		fprintf(stderr, "Failed to allocate buffer to keep track of which prompts were answered correctly.\n");
		return -1;
	}

	unsigned short random_index = 0;
	for(unsigned short i = 0; i < cs->length; i++)
	{
		// Reset input buffer length
		input_buffer_length = 0;
		input_c = 1;

		// Grab random card
		random_index = rand() % cs->length;
		while(visited_statuses_buffer[random_index] != false)		// only want unvisited cards
			random_index = rand() % cs->length;

		// Do not revisit random card
		visited_statuses_buffer[random_index] = true;

		// Prompt the user with the question
		printf("#%hu (%hu/%hu) %s\n> ", random_index, i + 1, cs->length, cs->cards[random_index]->front);

		// get user input
		while(input_c != '\n')
		{
			// Get a character from stdin
			input_c = getc(stdin);

			// Make sure buffer is big enough to append it
			if(input_buffer_length >= input_buffer_capacity)
			{
				input_buffer_capacity *= 2;
				input_buffer = realloc(input_buffer, input_buffer_capacity);
				if(input_buffer == NULL)
				{
					fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
					return 0;
				}
			}
			if(input_c == '\n')
				continue;
			
			input_buffer[input_buffer_length] = input_c;
			input_buffer_length++;
		}
		input_buffer[input_buffer_length] = '\0';
		input_buffer_length++;

		// Validate answer
		if(card_validate_answer(cs->cards[random_index], input_buffer))
		{
			printf("[CORRECT!]\n\n");
			correct_prompts[correct_counter] = random_index;
			correct_counter++;
		}
		else
			printf("[INCORRECT!]\n\n");
	
	}

	// Review user's performance
	printf("\nRECAP:\nYou answered %hu prompts correctly!\n", correct_counter);
	printf("You answered the following prompts correctly: %hu", correct_prompts[0]);
	for(unsigned short i = 1; i < correct_counter; i++)
		printf(", %hu", i);

	printf("\n");

	// Cleanup
	free(input_buffer);
	free(correct_prompts);
	free(visited_statuses_buffer);

	return correct_counter;
}

// Returns the amount of prompts answered correctly (incorrect amount can then be deduced)
// NOTE: This version uses an array of tuples for the incorrect prompts (instead of an array of shorts).
//       This is because additional information is required when reviewing prompts in back-first mode. 
//       The card index alone is insufficient, because we also need to know which alternate answer was used
//       as a prompt when the user answered incorrectly.
unsigned short card_set_play_back_first(struct card_set *cs, struct tuple *incorrect_prompts)
{
	// Setup for user input
	char *input_buffer = malloc(25);
	char input_c;
	size_t input_buffer_length = 0;
	size_t input_buffer_capacity = 25;

	// Revisit buffer --- which cards to revisit (those with alternative answers)
	// and which index to visit
	// (1) potentially every card has to be revisited (every single card has alt. answers).
	//     Use an array to represent index in cs->cards (idx 0) and the index within the card's
	//     alt. answers buffer to visit (idx 1)
	// (2) For convenience, it is assumed that all cards have alternative answers. This is not super
	//     likely, but it helps avoid needing to reallocate memory later. Heap memory is used, because
	//     a large card set could cause a stack overflow.
	struct tuple *revisit_buffer = malloc(sizeof(struct tuple) * cs->length);
	if(revisit_buffer == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for revisit buffer of card set \"%s\" with %hu cards.\n", cs->name, cs->length);
		free(input_buffer);
		return 0;
	}
	unsigned short revisit_buffer_length = 0;
	unsigned short revisit_buffer_index = 0;
	
	// Statistics
	unsigned short incorrect_counter = 0;

	// First run --- get all of the cards that need to be revisited
	for(unsigned short i = 0; i < cs->length; i++)
	{
		// Check if card should be inserted into the revisit buffer
		if(cs->cards[i]->alternate_answers_length > 0)
		{
			// Create a tuple within the revisit buffer.
			revisit_buffer[revisit_buffer_length] = malloc(sizeof(struct tuple));
			if(revisit_buffer[revisit_buffer_length] == NULL)
			{
				fprintf(stderr, "Failed to allocate memory for a tuple at index %hu of the revisit buffer while running through card set \"%s\" (length %hu). The index for the card set's cards buffer was %hu.\n", revisit_buffer_length, cs->name, cs->length, i);
				for(unsigned short j = 0; j < revisit_buffer_length; j++)
					free(revisit_buffer[j]);
				free(revisit_buffer);
				free(input_buffer);
				return 0; // CHANGE THIS LATER --- NEEDS TO REFLECT A PROPER ERROR VALUE.
			}

			// Setup tuple's information for first-run (index within alt. answer buffer should always be 0)
			revisit_buffer[revisit_buffer_length][0] = i;	// index in card set
			revisit_buffer[revisit_buffer_length][1] = 0;	// index in alt. answer buffer.
		}

		// Reset input buffer length
		input_buffer_length = 0;
		input_c = 1;

		// Prompt the user with the question (BACKSIDE, NOT FRONT)
	 	printf("(%hu/%hu) %s\n> ", i + 1, cs->length, cs->cards[i]->back);

		// get user input
		while(input_c != '\n')
		{
			// Get a character from stdin
			input_c = getc(stdin);

			// Make sure buffer is big enough to append it
			if(input_buffer_length >= input_buffer_capacity)
			{
				input_buffer_capacity *= 2;
				input_buffer = realloc(input_buffer, input_buffer_capacity);
				if(input_buffer == NULL)
				{
					fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
					return -1;
				}
			}
			if(input_c == '\n')
				continue;
			
			input_buffer[input_buffer_length] = input_c;
			input_buffer_length++;
		}
		input_buffer[input_buffer_length] = '\0';
		input_buffer_length++;

		// Validate answer (CHECKING FRONT FOR ANSWER)
		if(card_validate_answer_back(cs->cards[i], input_buffer))
			printf("[CORRECT!]\n\n");
		else
		{
			printf("[INCORRECT!]\n\n");

			// Show the user what the correct input was.
			// NOTE: there is only one correct answer, because we are checking the front
			//       side of the card
			printf("The correct answer was \"%s\"", cs->cards[i]->front);

			// Ask the user if they want to override the incorrect labeling.
			printf("\nOverride incorrect marker? [y/n]\n> ");

			// Get user input
			input_c = 1;
			input_buffer_length = 0;
			while(input_c != '\n')
			{
				// Get a character from stdin
				input_c = getc(stdin);

				// Make sure buffer is big enough to append it
				if(input_buffer_length >= input_buffer_capacity)
				{
					input_buffer_capacity *= 2;
					input_buffer = realloc(input_buffer, input_buffer_capacity);
					if(input_buffer == NULL)
					{
						fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
						// cleanup other items (input_buffer can be left alone, bc it is NULL)
						// (1) revisit buffer
						for(unsigned short j = 0; j < revisit_buffer_length; j++)
							free(revisit_buffer[j]);

						free(revisit_buffer);
						return 0;
					}
				}
				if(input_c == '\n')
					break;
				
				input_buffer[input_buffer_length] = input_c;
				input_buffer_length++;
			}
			input_buffer[input_buffer_length] = '\0';
			input_buffer_length++;

			// If no override, then mark the prompt as answered incorrectly.
			// Also, accept non-answer (buffer len = 1 from null terminator).
			if(input_buffer_length == 1 || input_buffer[0] == 'n')
			{
				incorrect_prompts[incorrect_counter] = i;
				incorrect_counter++;
			}

			printf("\n\n");
		}	
	}

	// Subsequent reruns --- Continue looping until there are no items in the revisit buffer.
	char *prompt;
	struct card *current_card;
	struct card **cards = cs->cards;
	byte alt_answer_index = 0;
	while(revisit_buffer_length != 0)
	{
		// Reset input buffer length
		input_buffer_length = 0;
		input_c = 1;

		// Gather data for current card to be revisited
		alt_answer_index = revisit_buffer[revisit_buffer_index][1];
		current_card = cards[revisit_buffer_index];
		prompt = current_card->alternate_answers[alt_answer_index];

		// Prompt the user with the question (BACKSIDE, NOT FRONT)
	 	printf("(%hu/%hu) %s\n> ", revisit_buffer_index + 1, revisit_buffer_length, prompt);

		// get user input
		while(input_c != '\n')
		{
			// Get a character from stdin
			input_c = getc(stdin);

			// Make sure buffer is big enough to append it
			if(input_buffer_length >= input_buffer_capacity)
			{
				input_buffer_capacity *= 2;
				input_buffer = realloc(input_buffer, input_buffer_capacity);
				if(input_buffer == NULL)
				{
					fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
					return -1;
				}
			}
			if(input_c == '\n')
				continue;
			
			input_buffer[input_buffer_length] = input_c;
			input_buffer_length++;
		}
		input_buffer[input_buffer_length] = '\0';
		input_buffer_length++;

		// Validate answer (CHECKING FRONT FOR ANSWER)
		if(card_validate_answer_back(current_card, input_buffer))
		{
			printf("[CORRECT!]\n\n");
		}
		else
		{
			printf("[INCORRECT!]\n\n");

			// Show the user what the correct input was.
			// NOTE: there is only one correct answer, because we are checking the front
			//       side of the card
			printf("The correct answer was \"%s\"", cs->cards[i]->front);

			// Ask the user if they want to override the incorrect labeling.
			printf("\nOverride incorrect marker? [y/n]\n> ");

			// Get user input
			input_c = 1;
			input_buffer_length = 0;
			while(input_c != '\n')
			{
				// Get a character from stdin
				input_c = getc(stdin);

				// Make sure buffer is big enough to append it
				if(input_buffer_length >= input_buffer_capacity)
				{
					input_buffer_capacity *= 2;
					input_buffer = realloc(input_buffer, input_buffer_capacity);
					if(input_buffer == NULL)
					{
						fprintf(stderr, "Failed to allocate extra memory for the input buffer.\n");
						// cleanup other items (input_buffer can be left alone, bc it is NULL)
						// (1) revisit buffer
						for(unsigned short j = 0; j < revisit_buffer_length; j++)
							free(revisit_buffer[j]);

						free(revisit_buffer);
						return 0;
					}
				}
				if(input_c == '\n')
					break;
				
				input_buffer[input_buffer_length] = input_c;
				input_buffer_length++;
			}
			input_buffer[input_buffer_length] = '\0';
			input_buffer_length++;

			// If no override, then mark the prompt as answered incorrectly.
			// Also, accept non-answer (buffer len = 1 from null terminator).
			if(input_buffer_length == 1 || input_buffer[0] == 'n')
			{
				incorrect_prompts[incorrect_counter] = i;
				incorrect_counter++;
			}

			printf("\n\n");
		}	
	}
	// Review user's performance
	printf("RECAP:\nYou answered %hu prompts correctly!\n", cs->length - incorrect_counter);
	printf("You answered the following prompts INCORRECTLY [%hu total]: %hu", incorrect_counter, incorrect_prompts[0] + 1); // +1 because the system is keeping track of indices; however, this information is presented to a human, who starts counting at 1.
	for(unsigned short i = 1; i < incorrect_counter; i++)
		printf(", %hu", incorrect_prompts[i] + 1);

	printf("\n");

	// Cleanup
	free(input_buffer);

	return incorrect_counter;
}

