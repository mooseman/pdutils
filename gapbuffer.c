

/*  gapbuffer.c                                 */  
/*  A simple gapbuffer implementation.          */ 
/*  This code is released to the public domain. */  
/*  "Share and enjoy..."  ;)                    */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>  


char *slice(char *array, int first, int last) 
{ 
    char *retarray = (char*) malloc(last-first);         
                       
    /*  Do the array slice  */          
    strncpy(retarray, array+first, last);
     
    return retarray;                      
    free(retarray);  
    retarray = NULL;                                                                    
}  


/* A pointer to move around the text */ 
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



/*  Print the text  */  
void print_text(char *text) 
{ 
   printf("Text is %s \n", text);   
}     


/*  Return the first text buffer */ 
char *buf1(char *text, int pos) 
{ 
   char *buffer1 = slice(text, 0, pos);
   return buffer1;    
}     


/*  Return the second text buffer */ 
char *buf2(char *text, int pos) 
{ 
   char *buffer2 = slice(text, pos, strlen(text) );
   return buffer2;    
}     


/*  Return the cursor position */  
void cursorpos() 
{ 
   

}  



/*  Create a gap in the middle of the text.  */ 
void makegap(char *text, int pos) 
{ 
     
    
    
}     


/*  Remove the gap (before moving it).  */  
void removegap(char *text) 
{
    
    
    
}     


/*  Move the cursor (and the gap).  */  
void movepos(char *text, int num)
{ 
     
    
    
}     



/* Test the code.  */
int main() 
{ 
    
  /* Create a text string to work with.  */  
  char *text = "Mary had a little lamb, its fleece was white as snow." ;              
  print_text(text);  
      
  char *buffer1 = buf1(text, 14);
  char *buffer2 = buf2(text, 14);
  
  printf("Buffer1: %s \n", buffer1); 
  printf("Buffer2: %s \n", buffer2);    
      
  char *test = ptr("obsidian", 3); 
  printf("Ptr: %s \n", test);  

  char *foo = "obsidian"; 
  /* Create another pointer by adding to foo. */ 
  char *bar = foo + 3;    
  
  /* Get the first part of the text */   
  char baz[4];
  strncpy(baz, foo, 3);
  /* Null-terminate the text */  
  baz[3]=0 ;   
      
  /* Get the middle of the string */  
  /* "*foo" is "obsidian".  */ 
  /* "*bar" is *foo + 3. "idian" in other words. */    
  char middle[4]; 
  strncpy(middle, bar, 3); 
  middle[3]=0;    
                
  int myindex = index(foo, bar);  
  printf("Index: %d \n", myindex);  
    
  printf("Foo: %s \n", foo);  
  printf("Bar: %s \n", bar);  
  printf("Baz: %s \n", baz);  
  printf("Middle: %s \n", middle);    
        
        
  return 0; 
        
}




    



