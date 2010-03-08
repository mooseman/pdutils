

/* pdawk.c */
 
/*
An awk-like program which parses data from the command-line. 
This code is released to the public domain.
"Share and enjoy....."  ;)   
*/
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
  
/*  An array of structs to hold our tokens  */    
typedef struct toklist { 
   int num; 
   char tok; 
} tokarray[20];  

     
int main(void)
{
  
  char text[80];
  char *tokens;
  
  tokarray myarray;  
              
  /*  Maximum length of line  */ 
  int maxlen=80; 
  int i=0; 
  
    puts("Enter some text :");
    fgets(text, maxlen, stdin);     
    tokens = strtok(text, " ");
    
    while (tokens != NULL) {

    printf("Token = %s\n", tokens);
    
    myarray[i].num = i; 
    myarray[i].tok = tokens; 
    
    i++;
    
    printf("Tokarray num = %d\n", myarray[i].num);
    /* printf("Tokarray token = %s\n", myarray[i].tok);  */  
    
    tokens = strtok(NULL, " ");    
            
    }      
      
  return 0;
 
}
 
 
 
 
