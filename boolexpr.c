

/* boolexpr.c */

/*  A simple program with a function which takes an integer,   */
/*  feeds it to a boolean expression, and returns whether the  */
/*  expression is true or false.                               */ 
/*  This code is released to the public domain.     */
/*  "Share and enjoy...."   ;)                      */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h> 
#include <math.h>


/* If cond is true, this function will return 1. */ 
/* Otherwise, it will return 0.  */    
bool test(int val, bool cond) 
{  
    return cond; 
}    
  


int main(void)
{
            
   int myval = 15;          
   /* This is true, so it will return 1. */              
   printf("Result is %d \n",  test(myval, (myval > 8) ) );
      
   int myval2 = 20;        
   /* This is false, so it will return 0. */              
   printf("Result is %d \n",  test(myval2, (myval2 == 30) ) );
      
   return 0 ;
   
}


    
