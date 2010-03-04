

/*  cmdhist.c  */  

/*                                                     
  A simple program to try to create a command history. */  
  
/*  This code is released to the public domain. */  
/*  "Share and enjoy...."  ;)    */   

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
     
void alt_a(void)
{ 
   printf("Hey, you pressed ALT a! \n"); 
}   

void alt_b(void)
{ 
   printf("Hey, you pressed ALT b! \n"); 
}   

void ctrl_a(void)
{ 
   printf("Hey, you pressed CTRL a! \n"); 
}   

void ctrl_g(void)
{ 
   printf("Hey, you pressed CTRL g! \n"); 
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


/*  Create a struct to store commands  */  
typedef struct cmds { 
    int index;
    char *cmd; 
} cmds;      

/* Create an array of the cmds struct. We will store 50 commands  */ 
struct cmdhist { 
    int size;      /*  Size of the array.  */  
    int n;         /*  Current no. of entries  */  
    cmds **history 
};  

/*  Create a command history struct  */  
cmdhist hist_create(int size) { 
    cmdhist *myhist = malloc(sizeof(cmdhist)); 
    cmds **mycmds; 
    int i; 
    
    if (myhist == NULL) 
       return NULL; 
       
    if (size <= 0) {
        free(myhist);
        return NULL;
    }
   
    myhist->size = size; 
    myhist->myhist = malloc(sizeof(cmds) * size ); 
    cmds = myhist->myhist; 
    
    if (cmds == NULL) { 
        free(myhist); 
        return NULL; 
    }   
    
    for (i=0; i < size; i++) 
       cmds[i] = NULL; 
    myhist->n = 0; 
            
    return myhist; 
}        




/*  Store the command history  */  
void storecmds(char *cmd) 
{  
   /* We will store 50 commands  */   
   
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
  
   else if (!strcmp(word, "\x1b\x61") ) { 
      alt_a(); 
  } 
  
   else if (!strcmp(word, "\x1b\x62") ) { 
      alt_b(); 
  } 
    
   else if (!strcmp(word, "\x07") ) { 
      ctrl_g(); 
  } 
  
   else if (!strcmp(word, "\x01") ) { 
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


         
