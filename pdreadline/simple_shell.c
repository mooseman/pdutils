

/*  simple_shell.c  */  
/*  An ultra-simple shell to test reading command history and */ 
/*  simple line editing. */  
/*  This code is based on ladsh4.c  */  
 

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h> 


 
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


/*  If the key pressed is an arrow key, backspace or a */ 
/* function key, do not echo it. Otherwise, echo it.   */       








int main() 
{
               
    char word[80];  
            
    while (1) {
    /*  Get keystrokes here  */  
     
    printf("@> ");                         
    scanf("%s", word);   
                                  
             
    if (!strcmp(word, "\x1b\x5b\x41") ) { 
    printf("You pressed the up arrow! \n"); }  
    
    else if (!strcmp(word, "quit") ) {            
      break; 
    }     
                         
 }

    return 0;
}


