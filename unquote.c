

/*  unquote.c  */  
/*  Remove quotes from around a string so that    */ 
/*  an expression can be used as a boolean expr.  */ 
/*  This code is released to the public domain.  */
/*  "Share and enjoy...."   ;)                   */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>  


bool unquote(int a, char *expr) 
{  
   char newexp[strlen(expr)];
   bool myexp; 
   int i; 

   for (i=0; i<strlen(expr); i++) 
   { 
      newexp[i] = expr[i]; 
   } 
   
   myexp = (bool)newexp;          
   return myexp;

} 


bool unquote2(char *expr) 
{ 
  
  bool retval = (bool)expr; 
  return retval;   
    
}     




int main(void)
{
         
   /* Test to see if an element is in the array.  */              
   printf("Result is %d \n",  unquote(7, "a + 5 > 20") );
      
   printf("Result is %d \n",  unquote2("7 + 5 > 20") );   
      
   return 0 ;
}






 


