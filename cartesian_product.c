

/*  cartesian_product.c  */  
/*  Given two coordinates, create two arrays */ 
/*  and find their cartesian product.  */ 
/*  This code is released to the public domain.  */ 
/*  "Share and enjoy..."  ;)                     */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* A position struct. This comes from the code for pdspread. */ 
typedef struct 
{ 
  int ypos; 
  int xpos;       
} pos;     
 
 
/* A cartesian product function. */ 
void cp(pos pos1, pos pos2) 
{ 
   int xdiff = pos2.xpos - pos1.xpos; 
   int ydiff = pos2.ypos - pos1.ypos;   
  
   /* Declare our array to hold the results.  */  
   int resarray[xdiff][ydiff];   
   
   int x, y;
       
   /* Having the y-loop outside the x-loop means that  */ 
   /* the operation proceeds row-by-row.               */                   
   for (y=pos1.ypos; y<pos2.ypos; y++)   
     for (x=pos1.xpos; x<pos2.xpos; x++)  
          { 
            resarray[x][0] = y;                 
            resarray[x][1] = x; 
            printf("(%d , %d) \n", resarray[x][0], resarray[x][1]);   
          }       
}     
    

int main(void) 
{ 
   
   pos mypos1, mypos2; 
   mypos1.xpos = 2; 
   mypos1.ypos = 5; 
   mypos2.xpos = 12; 
   mypos2.ypos = 8; 
   
   cp(mypos1, mypos2);   
      
   return 0 ; 
   
} 



 

