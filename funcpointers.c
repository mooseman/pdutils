

/* funcpointers.c  */ 
/* Playing around with function pointers.  */  
/*  This code is released to the public domain.      */ 
/*  "Share and enjoy...."  ;)                        */  



#include <stdio.h> 
#include <stdlib.h>
#include <stddef.h> 
#include <string.h>
#include <math.h> 


int foo(int arg)
{
   return arg * 5; 
} 


char bar(char *arg) 
{
   return arg[3]; 
} 


void baz(float arg) 
{ 
   printf("%2f \n", arg);  
}




int main()
{ 
            
   void (*myarr[]) (int, char *, float) = { foo, bar, baz } ; 
   
   
   
   
      
   return 0;
   
}



