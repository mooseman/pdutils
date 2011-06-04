/* funptr.c : PUBLIC DOMAIN - Jon Mayo - August 16, 2006
 * - You may remove any comments you wish, modify this code any way you wish,
 *   and distribute any way you wish.*/
/* function pointer example */
#include <stdio.h>

/* various handler functions we would like to point to */
static void hello(void) { printf("Hello"); }
static void world(void) { printf("World"); }
static void sp(void) { printf(" "); }
static void nl(void) { printf("\n"); }

/* array of function pointers. end with NULL */
static void (*call_table[])(void) = { hello, sp, world, nl, NULL }; 

/* you could make the table const doing this instead:
 * static void (*const call_table[])(void) 
 */

int main() { 
	int i;
	void (* x)(void); /* temp variable */

	/* loop through all entries until we see a NULL */
	for(i=0;call_table[i];i++) { 
		/* we could use call_table[i](); instead of a temp variable */
		x=call_table[i]; 
		x();
	}
	return 0;
} 
