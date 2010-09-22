

/*  test_io.c  */  

/*                                                     
  A small program to play around with input from files.  
  In particular, tokenising lines from files, and 
  taking action if a particular token is found.  
  Usage:  ./test_io  testfile.txt  
  
  This code is released to the public domain.                   
*/ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <ctype.h> 

#define TABSIZE 8 ; 

/*  A function to call if a given token */ 
/*  is found.  */  
void found(void)
{  
  printf("I found a 'small' token! \n"); 
}           
           
           
            
int main(int argc, char *argv[] ) 
{ 

  FILE *fp; 
  char line[80];
  char *tok;  
      
  if(argc < 2) { 
    printf("No filename entered.\n");
    exit(1);
    }  
    
  if ((fp=fopen(argv[1], "r"))==NULL) { 
    printf("Could not open file.\n");
    exit(1); 
    }  
   
   /*  Get lines from the file  */                  
   while ( fgets ( line, sizeof line, fp) != NULL )
   {   
     /*  Initialise the strtok function. */    
     tok = strtok(line, " ");  
     while (tok != NULL)  
        { 
          /*  Print the tokens separated by newlines.  */    
          printf("Token is %s \n", tok);  
          
          /*  Take action if a given token is found. */ 
          if ( !strcmp(tok, "small") ) { 
          found(); 
          }
             
          /* Now get the next token.  */       
          tok = strtok(NULL, " ");  
        }       
   }  
  
  /*  Close the file.  */  
  fclose(fp);        
  return 0; 

} 


         
     
     
       






   
   
    
 





