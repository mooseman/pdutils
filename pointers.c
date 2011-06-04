


/*  pointers.c  */  
/*  Playing around with pointers.  */   


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>  


/*  Get a slice from an array  */ 
char *slice(char *array, int first, int last) 
{ 
    char *retarray = (char*) malloc(last-first);         
                       
    /*  Do the array slice  */          
    strncpy(retarray, array+first, last);
     
    return retarray;                      
    free(retarray);  
    retarray = NULL;                                                                    
}  


/* A pointer to move around in a text string.  */ 
/* This works! Very handy to move around our text. */  
/* It returns the text from the letter pointed at */ 
/* to the end */  
char *ptr(char *text, int pos)
{ 
   return &text[pos];          
}     


/* Subtracting two pointers gives the number of */ 
/* array elements between them. We use that here. */  
int index(char *ptr1, char *ptr2)
{  
   int retval = ptr2 - ptr1; 
   return retval;       
}     



/* Test the code.  */
int main() 
{       
      
  char *test = ptr("obsidian", 3); 
  printf("Ptr: %s \n", test);  

  char *foo = "obsidian"; 
  /* Create another pointer by adding to foo. */ 
  char *bar = foo + 6;      
  /* And another pointer by subtracting from foo */   
  char *baz = bar - 2;
        
  printf("Foo: %s \n", foo);  
  printf("Bar: %s \n", bar);  
  printf("Baz: %s \n", baz);    
        
        
  return 0; 
        
}


