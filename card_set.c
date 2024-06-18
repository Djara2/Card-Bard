#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "card.h"
#include "card_set.h"

// Creation methods
struct card_set* card_set_create(char *name)
{
	struct card_set *new_card_set = malloc(sizeof(struct card_set));
	if(new_card_set == NULL)
	{
		fprintf(stderr, "Failed to create new card set with name \"%s\". Instantiation returned a NULL pointer.\n", name); 
		return NULL;
	}

	new_card_set->name_length = strlen(name);
	new_card_set->name = malloc(sizeof(char) * (new_card_set->name_length + 1));

	// Default size of 10 cards
	new_card_set->cards = malloc(sizeof(struct card*) * 10);
	new_card_set->length = 0;
	new_card_set->capacity = 10;

	return new_card_set;
}

// Modification methods
bool card_set_add(struct card_set *set, struct card *c)
{
	// Check if the set has reached its max size
	if(set->length >= 32767)
	{
		fprintf(stderr, "Card set \"%s\" cannot be resized, because its card buffer has reached 32,767 cards.\n", set->name);
		return false;
	}

	// Allocate more space if necessary
	if(set->length >= set->capacity)
	{
		// Add space for 50 more cards
		unsigned short new_capacity = set->capacity + 50;
		if(new_capacity > 32767)
			new_capacity = 32767;

		// Realloc buffer
		set->capacity = new_capacity;
		set->cards = realloc(set->cards, sizeof(struct card*) * new_capacity);
		if(set->cards == NULL)
		{
			fprintf(stderr, "Failed to allocate more cards to the cards buffer of card set \"%s\". Realloc returned a NULL pointer. Capacity set to %hu with length %hu.\n", set->name, set->capacity, set->length);
			return false;
		}
	}

	// Add the card
	set->cards[set->length] = c;
	set->length++;

	return true;
}

bool card_set_remove_by_key(struct card_set *set, char *key)
{
	for(unsigned short i = 0; i < set->length; i++)
	{
		// Current card does not match
		if(strcmp(set->cards[i]->key, key) != 0)
			continue;

		// Card found
		card_destroy(set->cards[i]); // Destroy card

		// Shift upper half of set down (left)
		for(unsigned short j = i + 1; j < set->length; j++)
			set->cards[j-1] = set->cards[j];

		set->length--;
		return true;		
	}
	return false;
}

bool card_set_remove_by_index(struct card_set *set, unsigned short index)
{
	if(index < 0)
	{
		fprintf(stderr, "Cannot remove card at index %hu from card set \"%s\". The index must be 0 or greater.\n", index, set->name);
		return false;
	}

	if(index >= set->length)
	{
		fprintf(stderr, "Cannot remove card at index %hu from card set \"%s\". The index is too large (the length parameter of a card set is human readable, e.g. cannot remove at index 13 for a length 13 array).", index, set->name);
		return false;
	}

	// Destroy card
	card_destroy(set->cards[index]);

	// Shift upper part (after index) down (left)
	for(unsigned short i = index + 1; i < set->length; i++)
		set->cards[i-1] = set->cards[i];
	
	set->length--;
	return true;
}

// File IO
bool card_set_write_binary(struct card_set *set)
{
	if(set == NULL)
	{
		fprintf(stderr, "Cannot write card set to a binary file. NULL pointer received for the card set passed as an argument.\n");
		return false;
	}

	if(set->name == NULL)
	{
		fprintf(stderr, "Cannot write card set to a binary file, because the char pointer for its name points to NULL.\n");
		return false;
	}

	char *file_name = malloc(sizeof(char) * (set->name_length + 5));
	strcpy(file_name, set->name);
	strcat(file_name, ".bin");
	FILE *fh = fopen(file_name, "wb");
	free(file_name);

	if(fh == NULL)
	{
		fprintf(stderr, "Failed to open file \"%s.bin\" for writing in binary mode.\n", set->name);
		return false;
	}
	// Write the file
	fwrite(&set->name_length, sizeof(unsigned char), 1, fh);	// Write size of name first so it can be allocated at read.
	fwrite(set->name, 1, set->name_length, fh);

	fwrite(&set->length, sizeof(unsigned short), 1, fh);	// Write length and capacity of the cards buffer first so it
	fwrite(&set->capacity, sizeof(unsigned short), 1, fh);   // can be allocated appropriately when reading.

	// Write each card that the card_set holds
	for(unsigned short i = 0; i < set->length; i++)
	{
		struct card *card = set->cards[i];

		// Write the card's key
		unsigned char key_length = strlen(card->key);		// Need to know amt. of chars to read later
		fwrite(&key_length, sizeof(unsigned short), 1, fh);
		fwrite(card->key, 1, key_length, fh);

		// Write the card's answers array
		fwrite(&card->values_length, sizeof(unsigned char), 1, fh);
		for(unsigned char j = 0; j < card->values_length; j++)
		{
			unsigned char value_length = strlen(card->values[j]);
			fwrite(&value_length, sizeof(unsigned char), 1, fh);
			fwrite(card->values[j], 1, value_length, fh);
		}
	}
	
	fclose(fh);
	return true;
}

bool card_set_write_csv(struct card_set *set)
{
	return true;
}

struct card_set* card_set_read_binary(char *file_name)
{
	FILE *fh = fopen(file_name, "rb");
	if(fh == NULL)
	{
		fprintf(stderr, "Failed to open file \"%s\" for reading in binary mode.\n", file_name);
		return NULL;
	}

	struct card_set *new_card_set = malloc(sizeof(struct card_set));
	if(new_card_set == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for the card set struct stored in file \"%s\".\n", file_name);
		fclose(fh);
		return NULL;
	}

	// Read length of name of card set
	if(fread(&new_card_set->name_length, sizeof(unsigned char), 1, fh) != 1)
	{
		fprintf(stderr, "Faild to read the length of the name for the card set to be created from file \"%s\".\n", file_name);
		free(new_card_set);
		fclose(fh);
		return NULL;
	}

	// Allocate enough space for name and read it in 
	new_card_set->name = malloc(new_card_set->name_length + 1);
	if(new_card_set->name == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for the name of the card set stored in file \"%s\".\n", file_name);
		fclose(fh);
		free(new_card_set);
		return NULL;
	}

	// Read the name of the card set
	if(fread(new_card_set->name, 1, new_card_set->name_length, fh) != new_card_set->name_length)
	{
		fprintf(stderr, "Failed to read the name of the new card set to be created from file \"%s\".\n", file_name);
		fclose(fh);
		free(new_card_set);
		return NULL;
	}
	new_card_set->name[new_card_set->name_length] = '\0';		// Null terminate the string

	// Read length of cards buffer
	if(fread(&new_card_set->length, sizeof(unsigned short), 1, fh) != 1)
	{
		fprintf(stderr, "Failed to read the length of the cards buffer for card set \"%s\" defined in file \"%s\".\n", new_card_set->name, file_name);
		free(new_card_set->name);
		free(new_card_set);
		fclose(fh);
		return NULL;
	}

	// Read capacity of cards buffer
	if(fread(&new_card_set->capacity, sizeof(unsigned short), 1, fh) != 1)
	{
		fprintf(stderr, "Failed to read the capacity of the cards buffer for card set \"%s\" defined in file \"%s\".\n", new_card_set->name, file_name);
		free(new_card_set->name);
		free(new_card_set);
		fclose(fh);
		return NULL;
	}

	// Allocate cards buffer
	new_card_set->cards = malloc(sizeof(struct card*) * new_card_set->capacity);
	if(new_card_set->cards == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for the cards buffer of the card set \"%s\" stored in file \"%s\".\n", new_card_set->name, file_name);
		free(new_card_set->name);
		free(new_card_set);
		fclose(fh);
		return NULL;
	}

	// Read each card into the buffer
	for(unsigned short i = 0; i < new_card_set->length; i++)
	{
		new_card_set->cards[i] = malloc(sizeof(struct card));
		if(new_card_set->cards[i] == NULL)
		{
			fprintf(stderr, "Failed to allocate memory for card number %hu in the card set \"%s\" defined in file \"%s\".\n", i, new_card_set->name, file_name);
			// Clean up previously allocated memory

			// Free previously allocated cards in cards buffer
			for(unsigned short j = 0; j < i; j++)		
			{					
				free(new_card_set->cards[j]->key);
				// Free the strings from the values buffer in the current card
				for(unsigned char k = 0; k < new_card_set->cards[j]->values_length; k++)
					free(new_card_set->cards[j]->values[k]);

				free(new_card_set->cards[j]->values);
				free(new_card_set->cards[j]);
			}

			// Free card set itself
			free(new_card_set->cards);
			free(new_card_set->name);
			free(new_card_set);
			fclose(fh);
			return NULL;
		}

		// Read the card's key length
		unsigned short key_length = 0;
		if(fread(&key_length, sizeof(unsigned short), 1, fh) != 1)
		{
			fprintf(stderr, "Failed to read the key length for card %hu in the card set \"%s\" defined in file \"%s\".\n", i, new_card_set->name, file_name);
			// Free previously allocated cards
			for(unsigned short j = 0; j < i; j++)
			{
				free(new_card_set->cards[j]->key);
				// Free values buffer of the current card
				for(unsigned char k = 0; k < new_card_set->cards[j]->values_length; k++)
					free(new_card_set->cards[j]->values[k]);
				
				free(new_card_set->cards[j]->values);
				free(new_card_set->cards[j]);
			}
			// Free the most recent card (i^th card), which will not have a values buffer or key yet.
			free(new_card_set->cards[i]);

			// Free the card set itself
			free(new_card_set->cards);
			free(new_card_set->name);
			free(new_card_set);
			fclose(fh);
			return NULL;
		}

		// Allocate memory for the card's key.
		new_card_set->cards[i]->key = malloc(key_length + 1);
		if(new_card_set->cards[i]->key == NULL)
		{
			fprintf(stderr, "Failed to allocate memory for key number %hu in the card set stored in file \"%s\".\n", i, file_name);
			// Partially free the card set
			free(new_card_set->name);

			// Free the cards thusfar created and put into into the card set
			for(unsigned short k = 0; k < i; k++)
			{
				free(new_card_set->cards[k]->key);

				// Free all the strings in the card's values buffer.
				for(unsigned char x = 0; x < new_card_set->cards[k]->values_length; x++)
					free(new_card_set->cards[k]->values[x]);

				// Free the value buffer and the card
				free(new_card_set->cards[k]->values);
				free(new_card_set->cards[k]);
			}

			// Free entire card set
			free(new_card_set);
			return NULL;
		}

		// Read the card's key itself.
		if(fread(new_card_set->cards[i]->key, 1, key_length, fh) != 1)
		{
			fprintf(stderr, "Failed to read key for card number %hu in card set \"%s\" defined in file \"%s\".\n", i, new_card_set->name, file_name);
			// Clean up cards which have been allocated thusfar
			for(unsigned short j = 0; j < i; j++)
			{
				free(new_card_set->cards[j]->key);
				// Free the values buffer of the current card
				for(unsigned char k = 0; k < new_card_set->cards[j]->values_length; k++)
					free(new_card_set->cards[j]->values[k]);
				
				free(new_card_set->cards[j]->values);
				free(new_card_set->cards[j]);
			}
			// Free the most recent card (needs to be freed differently, because it
			// will not have all the same stuff allocated)
			free(new_card_set->cards[i]->key);
			free(new_card_set->cards[i]);

			// Free the card set itself
			free(new_card_set->cards);
			free(new_card_set->name);
			free(new_card_set);
			
			fclose(fh);
			return NULL;
		}
		new_card_set->cards[i]->key[key_length] = '\0';		// Null terminate the read string

		// Read the length of the card's values buffer
		if(fread(&new_card_set->cards[i]->values_length, sizeof(unsigned char), 1, fh) != 1)
		{
			fprintf(stderr, "Failed to read the length of values buffer for card number %hu in the card set \"%s\" defined in file \"%s\".\n", i, new_card_set->name, file_name);
			// Clean up previously allocated cards
			for(unsigned short j = 0; j < i; j++)
			{
				free(new_card_set->cards[j]->key);
				// Free all the values in the values buffer of the current card
				for(unsigned char k = 0; k < new_card_set->cards[j]->values_length; k++)
					free(new_card_set->cards[j]->values[k]);
				
				free(new_card_set->cards[j]->values);
				free(new_card_set->cards[j]);
			}
			// Free the most recent card. This must be freed differently from the other ones
			// because it will have no values buffer yet.
			free(new_card_set->cards[i]->key);
			free(new_card_set->cards[i]);

			// Free the card set it self
			free(new_card_set->cards);
			free(new_card_set->name);
			free(new_card_set);

			fclose(fh);
			return NULL;
		}

		// Allocate memory for the card's values buffer
		new_card_set->cards[i]->values = malloc(sizeof(char*) * new_card_set->cards[i]->values_length);
		if(new_card_set->cards[i]->values == NULL)
		{
			fprintf(stderr, "Failed to allocate memory for the values buffer of the card \"%s\" in the card set \"%s\" stored in file \"%s\". The requested size was for %d char pointers.\n", new_card_set->cards[i]->key, new_card_set->name, file_name, new_card_set->cards[i]->values_length);
			
			// Clean up previously allocated memory
			free(new_card_set->cards[i]->key);		// (1) Free up card's key
			for(unsigned short j = 0; j <= i; j++)		// (2) Free all the cards within the card set
			{
				free(new_card_set->cards[j]->key);
				// Free all the values the card accepts for its key (prompt) 
				for(unsigned char k = 0; k < new_card_set->cards[j]->values_length; k++)
					free(new_card_set->cards[j]->values[k]);

				// Free values buffer and card itself
				free(new_card_set->cards[j]->values);
				free(new_card_set->cards[j]);
			}
			// Free the card set
			free(new_card_set->cards);
			free(new_card_set->name);
			free(new_card_set);
			fclose(fh);
			return NULL;
		}

		// Read values into the card's values buffer
		for(unsigned char j = 0; j < new_card_set->cards[i]->values_length; j++)
		{
			unsigned char value_length = 0;
			if(fread(&value_length, sizeof(unsigned char), 1, fh) != 1)
			{
				fprintf(stderr, "Failed to read the length of value number %d for values buffer in card number %hu for card set \"%s\" defined in file \"%s\".\n", j, i, new_card_set->name, file_name);

				// Free previously allocated cards
				for(unsigned short k = 0; k < i; k++)
				{
					free(new_card_set->cards[k]->key);
					// Free the current card's values buffer
					for(unsigned char l = 0; l < new_card_set->cards[k]->values_length; l++)
						free(new_card_set->cards[k]->values[l]);
					
					// Free the values buffer and the card itself
					free(new_card_set->cards[k]->values);
					free(new_card_set->cards[k]);
				}
				// Free the latest card, which must be freed differently from the other cards
				free(new_card_set->cards[i]->key);

				// Free however many values *could* be saved before an error occured.
				for(unsigned char k = 0; k < j; k++)
					free(new_card_set->cards[i]->values[k]);

				free(new_card_set->cards[i]->values);
				free(new_card_set->cards[i]);

				// Free card set itself
				free(new_card_set->name);
				free(new_card_set->cards);
				free(new_card_set);

				fclose(fh);
				return NULL;
			}

			// Allocate memory for the current value to be stored in the card's values buffer
			new_card_set->cards[i]->values[j] = malloc(value_length + 1);
			if(new_card_set->cards[i]->values[j] == NULL)
			{
				fprintf(stderr, "Failed to allocate memory for value %d in the values buffer of the card \"%s\" in the card set \"%s\" defined in file \"%s\".\n", j, new_card_set->cards[i]->key, new_card_set->name, file_name);
				// Clean up previously allocated memory
				for(unsigned short k = 0; k <= i; k++)
				{
					free(new_card_set->cards[k]->key);
					for(unsigned char l = 0; l < new_card_set->cards[k]->values_length; l++)
						free(new_card_set->cards[k]->values[l]);
					
					free(new_card_set->cards[k]->values);
					free(new_card_set->cards[k]);
				}
				free(new_card_set->cards);
				free(new_card_set->name);
				free(new_card_set);
				fclose(fh);
				return NULL;
			}
			if(fread(new_card_set->cards[i]->values[j], 1, value_length, fh) != value_length)
			{
				fprintf(stderr, "Failed to read value number %d for the values buffer of card number %hu in card set \"%s\" defined in file \"%s\".\n", j, i, new_card_set->name, file_name);
				// Clean up cards that have been allocated thusfar
				for(unsigned short k = 0; k <= i; k++)
				{
					free(new_card_set->cards[k]->key);
					// Free the strings from the values buffer in the current card
					for(unsigned char l = 0; l < new_card_set->cards[k]->values_length; l++)
						free(new_card_set->cards[k]->values[l]);

					// Free the values buffer and the card itself
					free(new_card_set->cards[k]->values);
					free(new_card_set->cards[k]);
				}
				// Free the card set itself
				free(new_card_set->cards);
				free(new_card_set->name);
				free(new_card_set);
				
				fclose(fh);
				return NULL;
			}
			new_card_set->cards[i]->values[j][value_length] = '\0';
		}	
	}

	fclose(fh);
	return new_card_set;
}

struct card_set* card_set_read_csv(char *file_name)
{
	return NULL;
}
