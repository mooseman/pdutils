


/*  keytest.c                                             */  
/*  Code to get the codes for keys when you press them.   */ 
/*  Very useful for getting codes for keys with           */ 
/*  two characters (e.g. arrow keys)                      */ 
/*  This code is released to the public domain.           */  
/*  "Share and enjoy....... "  ;)                         */   
  

#include <string.h>  
#include <stdio.h> 
#include <termios.h>  


/* Getch() from here - */ 
/* http://wesley.vidiqatch.org/   */ 
/* Thanks, Wesley!   */  
static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new = old; /* make new settings same as old settings */
    new.c_lflag &= ~ICANON; /* disable buffered i/o */
    new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) {
    tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) {
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}

/* Read 1 character without echo */
char getch(void) {
    return getch_(0);
}

/* Read 1 character with echo */
char getche(void) {
    return getch_(1);
} 



int main() 
{ 

 while(1) 
 { 
   
   int key = getch(); 
   printf("Key: %d \n", key); 
   key = getch(); 
   printf("Key: %d \n", key); 
        
 } 
 
return 0; 

} 














