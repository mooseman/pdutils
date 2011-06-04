

/*  This code is from Wikipedia, from here -       */ 
/*  http://en.wikipedia.org/wiki/Setcontext        */ 
/*  It was written by user Quuxplusone, who has    */ 
/*  dual-licensed all of his contributions as both */ 
/*  "public domain" and CC-A-SA */  
/*  ( See here -   */ 
/*  http://en.wikipedia.org/wiki/User:Quuxplusone     */  
/*  I therefore choose to go with his "public domain" */ 
/*  option, and so keep this code as "public domain". */  


#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
 
/* This is the iterator function. It is entered on the first call to
 * swapcontext, and loops from 0 to 9. Each value is saved in i_from_iterator,
 * and then swapcontext used to return to the main loop.  The main loop prints
 * the value and calls swapcontext to swap back into the function. When the end
 * of the loop is reached, the function exits, and execution switches to the
 * context pointed to by main_context1. */
void loop(
    ucontext_t *loop_context,
    ucontext_t *other_context,
    int *i_from_iterator)
{
    int i;
 
    for (i=0; i < 10; ++i) {
        /* Write the loop counter into the iterator return location. */
        *i_from_iterator = i;
 
        /* Save the loop context (this point in the code) into ''loop_context'',
         * and switch to other_context. */
        swapcontext(loop_context, other_context);
    }
 
    /* The function falls through to the calling context with an implicit
     * ''setcontext(&loop_context->uc_link);'' */
} 
 
int main(void)
{ 
    #define SIGSTKSZ 12  
    
    /* The three contexts:
     *    (1) main_context1 : The point in main to which loop will return.
     *    (2) main_context2 : The point in main to which control from loop will
     *                        flow by switching contexts.
     *    (3) loop_context  : The point in loop to which control from main will
     *                        flow by switching contexts. */
    ucontext_t main_context1, main_context2, loop_context;
 
    /* The stack for the iterator function. */
    char iterator_stack[SIGSTKSZ];
 
    /* Flag indicating that the iterator has completed. */
    volatile int iterator_finished;
 
    /* The iterator return value. */
    volatile int i_from_iterator;
 
    /* Initialise the iterator context. uc_link points to main_context1, the
     * point to return to when the iterator finishes. */
    loop_context.uc_link          = &main_context1;
    loop_context.uc_stack.ss_sp   = iterator_stack;
    loop_context.uc_stack.ss_size = sizeof(iterator_stack);
    getcontext(&loop_context);
 
    /* Fill in loop_context so that it makes swapcontext start loop. The
     * (void (*)(void)) typecast is to avoid a compiler warning but it is
     * not relevant to the behaviour of the function. */
    makecontext(&loop_context, (void (*)(void)) loop,
        3, &loop_context, &main_context2, &i_from_iterator);
 
    /* Clear the finished flag. */      
    iterator_finished = 0;
 
    /* Save the current context into main_context1. When loop is finished,
     * control flow will return to this point. */
    getcontext(&main_context1);
 
    if (!iterator_finished) {
        /* Set iterator_finished so that when the previous getcontext is
         * returned to via uc_link, the above if condition is false and the
         * iterator is not restarted. */
        iterator_finished = 1;
 
        while (1) {
            /* Save this point into main_context2 and switch into the iterator.
             * The first call will begin loop.  Subsequent calls will switch to
             * the swapcontext in loop. */
            swapcontext(&main_context2, &loop_context);
            printf("%d\n", i_from_iterator);
        }
    }
 
    return 0;
}




