

/*  simple_parser.c  */  

/*                                                     
  This is a very simple parser which uses strtok() to parse the 
  text entered by the user. It breaks up the text into tokens and 
  takes action accordingly.    
  This code is released to the public domain.                   
*/ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <ctype.h> 

    
int main(void) 
{ 
  
  char text[80]; 
  char *tokens;  
  
    puts("Enter some text :"); 
    scanf("%s", text);  
    tokens = strtok(text, " ");         
    tokens = strtok('\0', " "); 
    printf("|%s", tokens);                                                   
      
  return 0; 

} 


     


