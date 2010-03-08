

/* tokeniser2.c */
 
/*
A simple tokeniser which uses strtok() to split the 
text entered by the user. It breaks up the text into tokens, stores 
them in an array and prints them out. 
This code is released to the public domain.
"Share and enjoy....."  ;)   
*/
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
  
    
int main(void)
{
  
  char text[80];
  char *array[80];
  int i=0;
  char *token;
  /*  Maximum length of line  */ 
  int maxlen = 80; 
  
    puts("Enter some text :");
    fgets(text, maxlen, stdin);     
    token = strtok(text, " ");
    
    while (token != NULL) {

    printf("Token = %s\n", token);
    array[i] = token; 
    
    token = strtok(NULL, " ");    
        
    printf("Array %d is %s\n", i, array[i]);  
    
    i++; 
            
    }      
      
  return 0;
 
}
 
 
 
 
