


/*  keytest2.c                                            */  
/*  Code to get the codes for keys when you press them.   */ 
/*  Very useful for getting codes for keys with           */ 
/*  two characters (e.g. arrow keys)                      */ 
/*  This code is released to the public domain.           */  
/*  "Share and enjoy....... "  ;)                         */   
  

#include <string.h>  
#include <stdio.h> 
#include <stdlib.h> 
#include <stddef.h> 
#include <unistd.h> 


int main() 
{ 

   
 char *arr ;  
   

 while(1) 
 { 
      
   fgets(arr,80,stdin);    
   
   printf("Key: %s \n", arr);
   
   
   
 /* Look at having an integer array (buffer) to store input.     */ 
 /* This would let us read char-by-char. If we see a "27" (ESC)  */    
 /* we can scan a couple of ints ahead to see if we have got an  */ 
 /* arrow key, for example.                                      */  
         
 /* fgets(line, 80, stdin) ; 
      
   printf("Line: %s \n", line);  */  
        
 } 
   
 
return 0; 

} 




