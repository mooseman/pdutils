

/*  duh.c  */  

/*                                                     
  Duh is a "dummy shell". Really just a do-while loop to test 
  reading strings and acting on them.  
  This is eventually aimed at testing a public-domain readline 
  utility.   
  This code is released to the public domain.                   
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <ctype.h> 


void func1(void)
{  
   printf("Hey, you entered foo! \n"); 
} 

void func2(void) 
{  
   printf("Hey, you entered bar! \n"); 
} 
     

void ctrl_g(void)
{ 
   printf("Hey, you pressed CTRL g! \n"); 
}   

void ctrl_a(void)
{ 
   printf("Hey, you pressed CTRL a! \n"); 
}   

void up_arrow(void)
{ 
   printf("Hey, you pressed the up arrow! \n"); 
}   

void down_arrow(void)
{ 
   printf("Hey, you pressed the down arrow! \n"); 
}

void left_arrow(void)
{ 
   printf("Hey, you pressed the left arrow! \n"); 
}

void right_arrow(void)
{ 
   printf("Hey, you pressed the right arrow! \n"); 
} 

void f2(void)
{ 
   printf("Hey, you pressed F2! \n"); 
} 

void f3(void)
{ 
   printf("Hey, you pressed F3! \n"); 
} 

void f4(void)
{ 
   printf("Hey, you pressed F4! \n"); 
} 



    
int main(void) 
{ 
  
  char word[80]; 
  char ch; 
  
  do { 
    puts("Enter some text :"); 
    scanf("%s", word);  
    
   if ( !strcmp(word, "foo") ) { 
      func1(); 
  } 
  
   else if (!strcmp(word, "bar") ) { 
      func2(); 
  } 
  
   else if (!strcmp(word, "\007") ) { 
      ctrl_g(); 
  } 
  
   else if (!strcmp(word, "\001") ) { 
      ctrl_a(); 
  }
      
   else if (!strcmp(word, "\x1b\x5b\x41") ) { 
      up_arrow(); 
  }   
      
   else if (!strcmp(word, "\x1b\x5b\x42") ) { 
      down_arrow(); 
  }    
      
   else if (!strcmp(word, "\x1b\x5b\x43") ) { 
      right_arrow(); 
  }   
   
   else if (!strcmp(word, "\x1b\x5b\x44") ) { 
      left_arrow(); 
  } 
         
   else if (!strcmp(word, "\x1b\x4f\x51") ) { 
      f2(); 
  } 
   
   else if (!strcmp(word, "\x1b\x4f\x52") ) { 
      f3(); 
  }
  
   else if (!strcmp(word, "\x1b\x4f\x53") ) { 
      f4(); 
  }                    
                                                                                              
   else  { 
   printf("Nope - I do not recognise that phrase.... \n"); 
  }     
  
   printf("Try again? (y/n) : "); 
   scanf(" %c%*c", &ch);  
  } 
  
    while( toupper(ch) != 'N' );  
       
  return 0; 

} 


         
