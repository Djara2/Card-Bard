# Todo 

1. Playing through card sets in random order
2. Playing through card sets using the back of the card as the prompt (instead of the front)
	- Do not go through all alternative backsides at once.
		1. Define a queue (new card set) of cards to revisit.
		2. Go through all of the cards like normal
			1. If a card has alternate back prompts, put it in the queue of cards to revisit.
		3. Once the first playthrough of cards is done, go through the cards again, only selecting those from th queue of cards to revisit. Just call the card set playing function again from within the function, but provide the queue ofcards to revisit as the card set to iterate over.
3. Playing through card sets using the back of the card as the prompt (IN RANDOM ORDER)
4. Add unicode (hard)
5. Write a card set to a CSV
6. Application for modifying/creating card sets (in case the user does not want to use the text editor to modify the CSV for some reason).
7. GUI version

# Done

1. Ignoring whitespaces in around comma delimiter for CSV input file (card set). ALlow "front,back,alternative 1" to be written like "front, back, alternative 1", which is easier on the eyes.
