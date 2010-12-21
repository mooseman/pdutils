

/*  indent.c  */  

/*                                                     
  A small program to count the indentation of lines in a file.     
  This code is released to the public domain.                   
*/ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <ctype.h> 

#define TABSIZE 8 ; 

/*  Initialise linenum here  */  
int lnum = 0;  


/*  Find the indentation of a line  */  
int indent(char *line) 
{ 
   /*  Initialise indent counter.  */    
   int indent = 0; 
      
   /*  Loop counter  */ 
   int i; 
   
   for(i=0; i<strlen(line); i++) 
     {
      if ( line[i] == ' ' )
        { 
          indent += 1; 
        }     
      else if (line[i] == '\t' ) 
        { 
          indent += TABSIZE; 
        }                 
      else 
        { 
          break; 
        } 
     } 
            
    printf("Indent: %d \n", indent);  
    return indent; 
} 


/*  Function to return the line number  */  
int linenum(char *line) 
{
       
   if ( line )
        { 
          lnum += 1; 
        }        
               
    printf("Line: %d ", lnum);  
    return lnum; 
}  


            
int main(int argc, char *argv[] ) 
{ 

  FILE *fp; 
  char line[80]; 
  
  if(argc < 2) { 
    printf("No filename entered.\n");
    exit(1);
    }  
    
  if ((fp=fopen(argv[1], "r"))==NULL) { 
    printf("Could not open file.\n");
    exit(1); 
    }  
    
  while ( fgets ( line, sizeof line, fp) != NULL )
    {         
       linenum(line); 
       indent(line);  
    }

  fclose(fp);    
    
  return 0; 

} 


         
     
     
       






   
   
    
 





