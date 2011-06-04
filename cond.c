
/* cond.c */

/*  Returns TRUE if the condition is met, and    */
/*  FALSE otherwise.                             */
/*  This code is released to the public domain.  */
/*  "Share and enjoy...."   ;)                   */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h> 
#include <math.h>



/* Function to find if a value is a member  */ 
/* of an array.                             */    
/* Returns 1 if it is, and 0 if it is not.  */  
int myfunc(int myval, bool cond)
{          
    int retval; 
    
    if (cond) retval = 1;  
    else retval = 0;         
                  
    return retval;                                                                               
}



int main(void)
{
      
   int val1 = 5;    
      
   /* Test to see if an element is in the array.  */              
   printf("Result is %d \n",  myfunc(val1, (val1 - 7) < 8) );
      
   int val2 = 20; 
   printf("Result is %d \n",  myfunc(val2, (val2 - 7) < 8) );
   
   
   return 0 ;
}


    
