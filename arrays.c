// Example code from "a tutorial on 'dynamic' arrays in C"
// http://fydo.net

// Special thanks for suggestions and help from:
//   bbulkow of http://bbulkow.blogspot.com
//   tinkertim of http://echoreply.us

// Output:
// Steve's number is 42!
// Bill's number is 33!
// George's number is 15!
// fydo's number is 74!
//
// 6 allocated, 4 used

// This code is public domain. Do whatever you like with it! :D
// I've tested it using GCC, no promises for anything else. Sorry.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *name;
	int number;
} DATA;

DATA 	*the_array = NULL;
int 	num_elements = 0; // To keep track of the number of elements used
int		num_allocated = 0; // This is essentially how large the array is

int AddToArray (DATA item)
{
	if(num_elements == num_allocated) { // Are more refs required?
		
		// Feel free to change the initial number of refs and the rate at which refs are allocated.
		if (num_allocated == 0)
			num_allocated = 3; // Start off with 3 refs
		else
			num_allocated *= 2; // Double the number of refs allocated
		
		// Make the reallocation transactional by using a temporary variable first
		void *_tmp = realloc(the_array, (num_allocated * sizeof(DATA)));
		
		// If the reallocation didn't go so well, inform the user and bail out
		if (!_tmp)
		{ 
			fprintf(stderr, "ERROR: Couldn't realloc memory!\n");
			return(-1); 
		}
		
		// Things are looking good so far, so let's set the 
		the_array = (DATA*)_tmp;	
	}
	
	the_array[num_elements] = item; 
	num_elements++;
	
	return num_elements;
}

int main()
{
	// Some data that we can play with
	char *names[4] = { "Steve", "Bill", "George", "fydo" };
	int numbers[4] = { 42, 33, 15, 74 };
	int i;
	
	// Populate!
	for (i = 0; i < 4; i++)
	{
		DATA temp;
		
		temp.name = malloc((strlen(names[i]) + 1) * sizeof(char));
		strncpy(temp.name, names[i], strlen(names[i]) + 1);
		temp.number = numbers[i];
		
		if (AddToArray(temp) == -1) // If there was a problem adding to the array,
			return 1;				// we'll want to bail out of the program. You
									// can handle it however you wish.
	}
	
	// Open a file and ...
	FILE *out;
	out = fopen("output.txt", "w");
	
	// Regurgitate!
	for (i = 0; i < 4; i++)
	{
		fprintf(out, "%s's number is %d!\n", the_array[i].name, the_array[i].number);
	}
	
	fprintf(out, "\n%d allocated, %d used\n", num_allocated, num_elements);
	
	fclose(out);
	
	// Deallocate!
	for (i = 0; i < 4; i++)
	{
		free(the_array[i].name);
	}

	free(the_array);	
	
	// All done.
	return 0;
}
