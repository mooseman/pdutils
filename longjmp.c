

/* This code is from here - */ 
/* http://lawlor.cs.uaf.edu/~olawlor/ref/examples/unix/index.html */ 
/* Very many thanks to Orion Lawler for doing the examples there, and */ 
/* for placing them in the public domain. */   

/* Orion Lawlor's Short UNIX Examples, olawlor@acm.org 2008/04/21
Shows how to use setjmp/longjmp() for non-local control.
*/

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

jmp_buf to_main;
void doWork(int recurseCount)
{
	printf("Starting work %d:\n",recurseCount);
	if (recurseCount<=0) longjmp(to_main,1);
	else doWork(recurseCount-1);
	printf("Done with work %d:\n",recurseCount);
}


int main()
{
	if (setjmp(to_main)==0) {
		printf("Off to work!\n");
		doWork(4);
		printf("Back from work!\n");
	} else {
		printf("Back from work via a setjmp!\n");
	}
	return 0;
}

/*<@>
<@> ******** Program output: ********
<@> Off to work!
<@> Starting work 4:
<@> Starting work 3:
<@> Starting work 2:
<@> Starting work 1:
<@> Starting work 0:
<@> Back from work via a setjmp!
<@> */


