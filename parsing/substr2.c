

/* substr2.c  */  

/* Return a substring from a string.  */ 
/* This code is released to the public domain.     */ 
/* "Share and enjoy...."  ;)    */  


#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include <malloc.h> 


char *substr(char *str, int first, int last) 
{    
   char *ret = malloc(80);   
   int len = strlen(str); 
           
   if ( first < 1 || first > len || last < 1 || last > len ) 
     { puts("Freeing ret:"); free(ret); return "ERROR!" ; }         
                
   int i; 
   int j=0;   
   
   for (i=first-1; i<last; i++)  
   { 
      ret[j] = str[i];          
      j++;      
   } 
   
   ret[j] = '\0' ; 
   return ret; 
      
} 



int main() 
{ 
 
 char *test = "averylongstring"; 
          
 /* We will count the string starting from 1. */          
 /* So, this will return "very". */            
 char *result = substr(test, 2, 5) ;                          
 printf("%s \n", result) ; 
 
 /* This returns "string". */ 
 char *result2 = substr(test, 10, 15) ;                          
 printf("%s \n", result2) ; 
 
 /* This returns ERROR */ 
 char *result3 = substr(test, 0, 8) ;                          
 printf("%s \n", result3) ; 
 
 /* So does this. */  
 char *result4 = substr(test, 15, 18) ;                          
 printf("%s \n", result4) ; 
          
 free(result);  
 free(result2);  
 
 /* We do not need to free result3 and result4.  */ 
 /* They use the free in the error clause.       */  
 
                       
 return 0;
 
}  


