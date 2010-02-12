

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

/*  Previous indent  */  
int previndent ; 


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
            
    printf("Indent is %d \n", indent);  
    return indent; 
} 


/*  Function to return the line number  */  
int linenum(char *line) 
{
       
   if ( line )
        { 
          lnum += 1; 
        }        
               
    printf("Line number is %d \n", lnum);  
    return lnum; 
}  


/*  Create a struct to help us find the change in indentation */  
typedef struct delta_indent 
{ 
  int prevlinenum; 
  int previndent; 
  int curlinenum;
  int curindent; 
  int chg_indent; 
}      
     

/*  Function to find the change in indentation between lines  */  
struct change_indent(char *myline, int mylinenum)

 struct delta_indent d_indent ; 

 int myindent = indent(myline); 

  if (mylinenum == 0)   
  { 
     d_indent.prevlinenum = 0;
     d_indent.previndent = 0;
     d_indent.curlinenum = 0; 
     d_indent.curindent = myindent; 
     d_indent.chg_indent = 0;
  } 
  else 
  { 
     d_indent.prevlinenum = mylinenum-1;
     d_indent.previndent = xxx ; 
     d_indent.curlinenum = mylinenum; 
     d_indent.curindent = myindent; 
     d_indent.chg_indent = 0; 
        
    
    
    
      
  




            
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
       indent(line);  
       linenum(line); 
    }

  fclose(fp);    
    
  return 0; 

} 


         
     
     
       






   
   
    
 





